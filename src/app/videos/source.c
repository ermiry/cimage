#include <stdlib.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>

#include "app/videos/source.h"

static VideoSource *video_source_new (void) {

    VideoSource *source = (VideoSource *) malloc (sizeof (VideoSource));
    if (source) {
        source->avio_ctx = NULL;
        source->format_ctx = NULL;
    }

    return source;

}

static void video_source_delete (void *source_ptr) {

    if (source_ptr) {
        VideoSource *source = (VideoSource *) source_ptr;
        free (source);
    }

}