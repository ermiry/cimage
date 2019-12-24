#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "cengine/utils/utils.h"
#include "cengine/utils/log.h"

#include "app/videos/decoder.h"
#include "app/videos/source.h"
#include "app/videos/buffers/buffer.h"

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
        Decoder *dec = (Decoder *) decoder_ptr;

        if (dec->dec_close) dec->dec_close(dec);

        for (int i = 0; i < DEC_BUF_COUNT; i++) buffer_destroy (dec->buffer[i]);

        SDL_DestroyMutex (dec->output_lock);
        avcodec_close (dec->codec_ctx);
        avcodec_free_context (&dec->codec_ctx);

        free (dec);
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
            char *status = c_string_create ("decoder_create () - Invalid stream %d", stream_index);
            if (status) {
                #ifdef CIMAGE_DEBUG
                cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, status);
                #endif
                free (status);
            }
        }

        else {
            dec = decoder_new ();
            if (dec) {
                codec = avcodec_find_decoder (format_ctx->streams[stream_index]->codecpar->codec_id);
                if (!codec) {
                    char *status = c_string_create ("decoder_create () - No suitable decoder found for stream %d", stream_index);
                    if (status) {
                        #ifdef CIMAGE_DEBUG
                        cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, status);
                        #endif
                        free (status);
                    }

                    error = true;
                }

                else {
                    // Allocate a context for the codec
                    codec_ctx = avcodec_alloc_context3 (codec);
                    if (codec_ctx == NULL) {
                        char *status = c_string_create ("decoder_create () - Unable to allocate codec context for stream %d", stream_index);
                        if (status) {
                            #ifdef CIMAGE_DEBUG
                            cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, status);
                            #endif
                            free (status);
                        }

                        error = true;
                    }

                    else {
                        if (avcodec_parameters_to_context (codec_ctx, format_ctx->streams[stream_index]->codecpar) < 0) {
                            char *status = c_string_create ("decoder_create () - Unable to copy codec context for stream %d", stream_index);
                            if (status) {
                                #ifdef CIMAGE_DEBUG
                                cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, status);
                                #endif
                                free (status);
                            }

                            error = true;
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
                                av_dict_set (&codec_opts, "sub_text_format", "ass", 0);
                            #endif

                            // Open the stream
                            if (avcodec_open2(codec_ctx, codec, &codec_opts) < 0) {
                                char *status = c_string_create ("decoder_create () - Unable to open codec for stream %d", stream_index);
                                if (status) {
                                    #ifdef CIMAGE_DEBUG
                                    cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, status);
                                    #endif
                                    free (status);
                                }

                                error = true;
                            }

                            // Set index and codec
                            dec->stream_index = stream_index;
                            dec->codec_ctx = codec_ctx;
                            dec->format_ctx = format_ctx;

                            // Allocate input/output ringbuffers
                            for (int i = 0; i < 2; i++) {
                                dec->buffer[i] = buffer_create (bsizes[i], free_hooks[i]);
                                if (dec->buffer[i] == NULL) {
                                    char *status = c_string_create ("Unable to allocate buffer for stream %d", stream_index);
                                    if (status) {
                                        #ifdef CIMAGE_DEBUG
                                        cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, status);
                                        #endif
                                        free (status);
                                    }
                                    error = true;
                                }
                            }

                            // Create a lock for output buffer synchronization
                            dec->output_lock = SDL_CreateMutex ();
                            if (dec->output_lock == NULL) {
                                char *status = c_string_create ("Unable to allocate mutex for stream %d", stream_index);
                                if (status) {
                                    #ifdef CIMAGE_DEBUG
                                    cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, status);
                                    #endif
                                    free (status);
                                }
                                
                                #ifdef CIMAGE_DEBUG
                                cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, 
                                    SDL_GetError ());
                                #endif
                                error = true;
                            }
                        }
                    }
                }
            }
        }

        if (error) {
            for (int i = 0; i < DEC_BUF_COUNT; i++) buffer_destroy (dec->buffer[i]);

            if (codec_ctx) avcodec_close (codec_ctx);
            if (codec_opts) av_dict_free (&codec_opts);
            if (codec_ctx) avcodec_free_context (&codec_ctx);

            decoder_delete (dec);
            dec = NULL;
        }
    }

    return dec;

}

void decoder_close (Decoder *dec) {

    if (dec) decoder_delete (dec);

}

#pragma region output buffer

int decoder_write_output (Decoder *dec, void *packet) {

	int retval = 1;

	if (dec && packet) {
		if (SDL_LockMutex (dec->output_lock) == 0) {
			retval = buffer_write (dec->buffer[DEC_BUF_OUT], packet);
			SDL_UnlockMutex (dec->output_lock);
		}
	}

    return retval;

}

bool decoder_can_write_output (Decoder *dec) {

	bool retval = false;

    if (dec) {
		if (SDL_LockMutex (dec->output_lock) == 0) {
			retval = !buffer_is_full (dec->buffer[DEC_BUF_OUT]);
			SDL_UnlockMutex (dec->output_lock);
		}
    }
    
    return retval;

}

#pragma endregion