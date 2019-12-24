#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <SDL2/SDL_audio.h>

#include <libavformat/avformat.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>

#include "cengine/utils/utils.h"
#include "cengine/utils/log.h"

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

static void audio_dec_delete (AudioDecoder *audio_dec) {

	if (audio_dec) {
		if (audio_dec->scratch_frame != NULL) {
			av_frame_free (&audio_dec->scratch_frame);
		}

		if (audio_dec->swr != NULL) {
			swr_free(&audio_dec->swr);
		}

		free (audio_dec);
	}

}

typedef struct AudioPacket {

	double pts;
	size_t original_size;
	RingBuffer *rb;

} AudioPacket;

static AudioPacket *audio_packet_new (void) {

	AudioPacket *p = (AudioPacket *) malloc (sizeof (AudioPacket));
	if (p) {
		memset (p, 0, sizeof (AudioPacket));

		p->rb = NULL;
	}

	return p;

}

static void auido_packet_delete (AudioPacket *p) {

	if (p) {
		ring_buffer_destroy (p->rb);
		free (p);
	}

}

static AudioPacket *auido_packet_create (const char *data, size_t len, double pts) {

	AudioPacket *p = audio_packet_new ();
	if (p) {
		p->rb = ring_buffer_create (len);
		ring_buffer_write (p->rb, data, len);
		p->pts = pts;
	}

	return p;

}

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

static void dec_read_audio (Decoder *dec) {

	if (dec) {
		AudioDecoder *audio_dec = dec->userdata;
		int len;
		int dst_linesize;
		int dst_nb_samples;
		int dst_bufsize;
		double pts;
		unsigned char **dst_data;
		AudioPacket *out_packet = NULL;
		int ret = 0;

		// Pull decoded frames out when ready and if we have room in decoder output buffer
		while (!ret && decoder_can_write_output (dec)) {
			ret = avcodec_receive_frame (dec->codec_ctx, audio_dec->scratch_frame);
			if(!ret) {
				dst_nb_samples = av_rescale_rnd (
					audio_dec->scratch_frame->nb_samples,
					dec->output.samplerate,  // Target samplerate
					dec->codec_ctx->sample_rate,  // Source samplerate
					AV_ROUND_UP);

				av_samples_alloc_array_and_samples (
					&dst_data,
					&dst_linesize,
					dec->output.channels,
					dst_nb_samples,
					_FindAVSampleFormat (dec->output.format),
					0);

				len = swr_convert (
					audio_dec->swr,
					dst_data,
					dst_nb_samples,
					(const unsigned char **)audio_dec->scratch_frame->extended_data,
					audio_dec->scratch_frame->nb_samples);

				dst_bufsize = av_samples_get_buffer_size (
					&dst_linesize,
					dec->output.channels,
					len,
					_FindAVSampleFormat (dec->output.format), 1);

				// Get presentation timestamp
				pts = audio_dec->scratch_frame->best_effort_timestamp;
				pts *= av_q2d (dec->format_ctx->streams[dec->stream_index]->time_base);

				// Lock, write to audio buffer, unlock
				// TODO: 23//12/2019 -- 11:16 -- possible memory leak here if we dont delete the packet
				out_packet = auido_packet_create (
					(char *) dst_data[0], (size_t)dst_bufsize, pts);
				decoder_write_output (dec, out_packet);

				// Free temps
				av_freep (&dst_data[0]);
				av_freep (&dst_data);
			}
		}
	}

}

static int dec_decode_audio_cb (Decoder *dec, AVPacket *in_packet) {

	if (dec && in_packet) {
		// Try to clear the buffer first. We might have too much content in the ffmpeg buffer,
		/// so we want to clear it of outgoing data if we can.
		dec_read_audio (dec);

		// Write packet to the decoder for handling.
		if (avcodec_send_packet (dec->codec_ctx, in_packet) < 0) {
			return 1;
		}

		// Some input data was put in succesfully, so try again to get frames.
		dec_read_audio (dec);

		return 0;
	}

	return 1;

}

static void dec_close_audio_cb (Decoder *dec) {

	if (dec) audio_dec_delete ((AudioDecoder *) dec->userdata);
    
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
				bool error = false;

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
                        #ifdef CIMAGE_DEBUG
						char *status = c_string_create ("auido_create_decoder () - Unable to initialize audio resampler context");
						if (status) {
							cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, status);
							free (status);
						}
						#endif
                    }
                }

                else {
					#ifdef CIMAGE_DEBUG
					char *status = c_string_create ("auido_create_decoder () - Unable to initialize temporary audio frame");
					if (status) {
						cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, status);
						free (status);
					}
					#endif
                }

				if (error) {
					audio_dec_delete (audio_dec);
					
					decoder_close (dec);
					dec = NULL;
				}
            }

            else {
				#ifdef CIMAGE_DEBUG
				char *status = c_string_create ("auido_create_decoder () - Failed to allocate new audio decoder");
				if (status) {
					cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, status);
					free (status);
				}
				#endif

				decoder_close (dec);
				dec = NULL;
            }
        }
    }

    return dec;

}