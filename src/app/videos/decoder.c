#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "cengine/utils/utils.h"
#include "cengine/utils/log.h"

#include "app/videos/decoder.h"
#include "app/videos/source.h"

#define BUFFER_IN_SIZE                  256

static Decoder *decoder_new (void) {

    Decoder *decoder = (Decoder *) malloc (sizeof (Decoder));
    if (decoder) {
        memset (decoder, 0, sizeof (Decoder));

        decoder->codec_ctx = NULL;
        decoder->format_ctx = NULL;

        decoder->output_lock = NULL;
        decoder->buffer[0] = NULL;
        decoder->buffer[1] = NULL;

        decoder->userdata = NULL;
    }

    return decoder;

}

static void decoder_delete (void *decoder_ptr) {

    if (decoder_ptr) {
        Decoder *decoder = (Decoder *) decoder_ptr;

        free (decoder);
    }

}

static void free_in_video_packet_cb (void *packet) {

    av_packet_free ((AVPacket**) &packet);

}

Decoder *decoder_create (const VideoSource *src, int stream_index, 
    int out_b_size, dec_free_packet_cb free_out_cb, int thread_count) {

    Decoder *dec = NULL;
    bool error = false;

    if (src && (out_b_size > 0) && (thread_count > 0)) {
        AVCodecContext *codec_ctx = NULL;
        AVDictionary *codec_opts = NULL;
        AVCodec *codec = NULL;
        AVFormatContext *format_ctx = src->format_ctx;
        int bsizes[2] = { BUFFER_IN_SIZE, out_b_size };
        dec_free_packet_cb free_hooks[2] = { free_in_video_packet_cb, free_out_cb };

        // Make sure index seems correct
        if (stream_index >= (int) format_ctx->nb_streams || stream_index < 0) {
            char *status = c_string_create ("decoder_create () - invalid stream %d", stream_index);
            if (status) {
                cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, status);
                free (status);
            }
            // FIXME: error!
            // Kit_SetError("Invalid stream %d", stream_index);
            // goto exit_0;
        }

        else {
            dec = decoder_new ();
            if (dec) {
                codec = avcodec_find_decoder (format_ctx->streams[stream_index]->codecpar->codec_id);
                if (!codec) {
                    // FIXME: ERROR!
                }

                else {
                    // Allocate a context for the codec
                    codec_ctx = avcodec_alloc_context3(codec);
                    if (codec_ctx == NULL) {
                        // Kit_SetError("Unable to allocate codec context for stream %d", stream_index);
                        // goto exit_1;
                    }

                    else {
                        if (avcodec_parameters_to_context(codec_ctx, format_ctx->streams[stream_index]->codecpar) < 0) {
                            // FIXME: error!
                        }

                        else {
                            // Required by ffmpeg for now when using the new API.
                            #if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(57, 48, 101)
                                codec_ctx->pkt_timebase = format_ctx->streams[stream_index]->time_base;
                            #endif

                            // Set thread count
                            codec_ctx->thread_count = thread_count;
                            codec_ctx->thread_type = FF_THREAD_SLICE | FF_THREAD_FRAME;

                            // This is required for ass_process_chunk() support
                            #if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(57, 25, 100)
                                av_dict_set(&codec_opts, "sub_text_format", "ass", 0);
                            #endif

                            // Open the stream
                            if(avcodec_open2(codec_ctx, codec, &codec_opts) < 0) {
                                // FIXME: error!
                                // Kit_SetError("Unable to open codec for stream %d", stream_index);
                                // goto exit_2;
                            }

                            // Set index and codec
                            dec->stream_index = stream_index;
                            dec->codec_ctx = codec_ctx;
                            dec->format_ctx = format_ctx;

                            // Allocate input/output ringbuffers
                            for(int i = 0; i < 2; i++) {
                                dec->buffer[i] = Kit_CreateBuffer(bsizes[i], free_hooks[i]);
                                if(dec->buffer[i] == NULL) {
                                    // FIXME: ERROR!
                                    // Kit_SetError("Unable to allocate buffer for stream %d: %s", stream_index, SDL_GetError());
                                    // goto exit_3;
                                }
                            }

                            // Create a lock for output buffer synchronization
                            dec->output_lock = SDL_CreateMutex();
                            if(dec->output_lock == NULL) {
                                // FIXME: ERROR!
                                // Kit_SetError("Unable to allocate mutex for stream %d: %s", stream_index, SDL_GetError());
                                // goto exit_3;
                            }
                        }
                    }
                }
            }
        }
    }

    if (error) {
        // FIXME: correctly free memory!
    }

    return dec;

}