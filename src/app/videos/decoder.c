#include <stdlib.h>
#include <string.h>

#include "app/videos/decoder.h"

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