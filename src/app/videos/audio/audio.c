#include <stdlib.h>

#include <SDL2/SDL_audio.h>

#include <libavformat/avformat.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>

#include "app/videos/format.h"
#include "app/videos/decoder.h"
#include "app/videos/source.h"
#include "app/videos/buffers/ring.h"

typedef struct AudioDecoder {

    SwrContext *swr;
    AVFrame *scratch_frame;

} AudioDecoder;

static AudioDecoder *audio_dec_new (void) {

    AudioDecoder *audio_dec = (AudioDecoder *) malloc (sizeof (AudioDecoder));
    if (audio_dec) {
        audio_dec->swr = NULL;
        audio_dec->scratch_frame = NULL;
    }

    return audio_dec;

}

typedef struct AudioPacket {

    double pts;
    size_t original_size;
    RingBuffer *rb;

} AudioPacket;

static void free_out_audio_packet_cb (void *packet) {

    if (packet) {
        AudioPacket *p = (AudioPacket *) packet;
        ring_buffer_destroy (p->rb);
        free (p);
    }

}

static enum AVSampleFormat _FindAVSampleFormat (int format) {

    switch (format) {
        case AUDIO_U8: return AV_SAMPLE_FMT_U8;
        case AUDIO_S16SYS: return AV_SAMPLE_FMT_S16;
        case AUDIO_S32SYS: return AV_SAMPLE_FMT_S32;
        default: return AV_SAMPLE_FMT_NONE;
    }

}

static int64_t _FindAVChannelLayout (int channels) {

    switch (channels) {
        case 1: return AV_CH_LAYOUT_MONO;
        case 2: return AV_CH_LAYOUT_STEREO;
        default: return AV_CH_LAYOUT_STEREO_DOWNMIX;
    }
    
}

static int _FindChannelLayout (uint64_t channel_layout) {

    switch (channel_layout) {
        case AV_CH_LAYOUT_MONO: return 1;
        case AV_CH_LAYOUT_STEREO: return 2;
        default: return 2;
    }

}

static int _FindBytes (enum AVSampleFormat fmt) {

    switch (fmt) {
        case AV_SAMPLE_FMT_U8P:
        case AV_SAMPLE_FMT_U8:
            return 1;
        case AV_SAMPLE_FMT_S32P:
        case AV_SAMPLE_FMT_S32:
            return 4;
        default:
            return 2;
    }

}

static int _FindSDLSampleFormat (enum AVSampleFormat fmt) {

    switch (fmt) {
        case AV_SAMPLE_FMT_U8P:
        case AV_SAMPLE_FMT_U8:
            return AUDIO_U8;
        case AV_SAMPLE_FMT_S32P:
        case AV_SAMPLE_FMT_S32:
            return AUDIO_S32SYS;
        default:
            return AUDIO_S16SYS;
    }

}

static int _FindSignedness (enum AVSampleFormat fmt) {

    switch (fmt) {
        case AV_SAMPLE_FMT_U8P:
        case AV_SAMPLE_FMT_U8:
            return 0;
        default:
            return 1;
    }

}

Decoder *auido_create_decoder (const VideoSource *src, int stream_idx) {

    Decoder *dec = NULL;

    if (src && (stream_idx >= 0)) {
        // create the generic decoder
        // FIXME: pass correct values
        dec = decoder_create (src, stream_idx, 0, free_out_audio_packet_cb, 0);
        if (dec) {
            AudioDecoder *audio_dec = audio_dec_new ();
            if (audio_dec) {
                // Create temporary audio frame
                audio_dec->scratch_frame = av_frame_alloc ();
                if (audio_dec->scratch_frame) {
                    // Set format configs
                    OutputFormat output;
                    memset (&output, 0, sizeof (OutputFormat));
                    output.samplerate = dec->codec_ctx->sample_rate;
                    output.channels = _FindChannelLayout (dec->codec_ctx->channel_layout);
                    output.bytes = _FindBytes (dec->codec_ctx->sample_fmt);
                    output.is_signed = _FindSignedness (dec->codec_ctx->sample_fmt);
                    output.format = _FindSDLSampleFormat (dec->codec_ctx->sample_fmt);

                    // create resampler
                    audio_dec->swr = swr_alloc_set_opts (
                        NULL,
                        _FindAVChannelLayout (output.channels), // Target channel layout
                        _FindAVSampleFormat (output.format), // Target fmt
                        output.samplerate, // Target samplerate
                        dec->codec_ctx->channel_layout, // Source channel layout
                        dec->codec_ctx->sample_fmt, // Source fmt
                        dec->codec_ctx->sample_rate, // Source samplerate
                        0, NULL);

                    if (!swr_init (audio_dec->swr)) {
                        // Set callbacks and userdata, and we're go
                        dec->dec_decode = dec_decode_audio_cb;
                        dec->dec_close = dec_close_audio_cb;
                        dec->userdata = audio_dec;
                        dec->output = output;
                        // success!!
                    }

                    else {
                        // FIXME: error!
                    }
                }

                else {
                    // FIXME: error
                }
            }

            else {
                // FIXME: error!
            }
        }
    }

    return dec;

}