#include <libavformat/avformat.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>

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