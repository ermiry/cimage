#include <stdlib.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>

#include "cengine/utils/log.h"
#include "cengine/utils/utils.h"

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

// returns 0 on success, 1 on error
static int video_source_scan (AVFormatContext *format_ctx) {

    int retval = 1;

    if (format_ctx) {
        av_opt_set_int (format_ctx, "probesize", INT_MAX, 0);
        av_opt_set_int (format_ctx, "analyzeduration", INT_MAX, 0);
        if (avformat_find_stream_info (format_ctx, NULL) < 0) {
            #ifdef CIMAGE_DEBUG
            cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, 
                "Unable to fetch source information.");
            #endif
        }

        else retval = 0;
    }

    return retval;

}

VideoSource *video_source_create (const char *filename) {

    VideoSource *src = NULL;

    if (filename) {
        src = video_source_new ();

        // open source
        if (avformat_open_input ((AVFormatContext **) &src->format_ctx, filename, NULL, NULL) < 0) {
            #ifdef CIMAGE_DEBUG
            char *status = c_string_create ("Unable to open video source: %s", filename);
            if (status) {
                cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, status);
                free (status);
            }
            #endif

            video_source_delete (src);
            src = NULL;
        }

        // scan source information (may seek forwards)
        if (video_source_scan ((AVFormatContext *) src->format_ctx)) {
            avformat_close_input ((AVFormatContext **) &src->format_ctx);
            video_source_delete (src);
            src = NULL;
        }
    }

    return src;

}

void video_source_close (VideoSource *src) {

    if (src) {
        AVFormatContext *format_ctx = src->format_ctx;
        AVIOContext *avio_ctx = src->avio_ctx;
        avformat_close_input (&format_ctx);
        if (avio_ctx) {
            av_freep (&avio_ctx->buffer);
            av_freep (&avio_ctx);
        }

        video_source_delete (src);
    }

}

int video_source_get_stream_count (const VideoSource *src) {

    if (src) return ((AVFormatContext *) src->format_ctx)->nb_streams;

}

// returns -1 on error
int video_source_get_best_stream (const VideoSource *src, const StreamType type) {

    int retval = -1;

    if (src) {
        int avmedia_type = 0;
        switch (type) {
            case STREAMTYPE_VIDEO: avmedia_type = AVMEDIA_TYPE_VIDEO; break;
            case STREAMTYPE_AUDIO: avmedia_type = AVMEDIA_TYPE_AUDIO; break;
            case STREAMTYPE_SUBTITLE: avmedia_type = AVMEDIA_TYPE_SUBTITLE; break;
            default: return -1;
        }

        retval = av_find_best_stream ((AVFormatContext *) src->format_ctx, avmedia_type, -1, -1, NULL, 0);

        if (retval == AVERROR_STREAM_NOT_FOUND) retval = -1;

        if (retval == AVERROR_DECODER_NOT_FOUND) {
            #ifdef CIMAGE_DEBUG
            cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, "Unable to find a decoder for the stream.");
            #endif
            retval = -1;
        }
    }

    return retval;

}

// returns 0 on success, 1 on error
int video_source_get_stream_info (const VideoSource *src, StreamInfo *info, int idx) {

    int retval = 1;

    if (src && info) {
        AVFormatContext *format_ctx = (AVFormatContext *) src->format_ctx;
        if (idx < 0 || idx >= format_ctx->nb_streams) {
            #ifdef CIMAGE_DEBUG
            cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, "Invalid stream index.");
            #endif
        } 

        else {
            AVStream *stream = format_ctx->streams[idx];
            enum AVMediaType codec_type;
            codec_type = stream->codecpar->codec_type;

            switch (codec_type) {
                case AVMEDIA_TYPE_UNKNOWN: info->type = STREAMTYPE_UNKNOWN; break;
                case AVMEDIA_TYPE_DATA: info->type = STREAMTYPE_DATA; break;
                case AVMEDIA_TYPE_VIDEO: info->type = STREAMTYPE_VIDEO; break;
                case AVMEDIA_TYPE_AUDIO: info->type = STREAMTYPE_AUDIO; break;
                case AVMEDIA_TYPE_SUBTITLE: info->type = STREAMTYPE_SUBTITLE; break;
                case AVMEDIA_TYPE_ATTACHMENT: info->type = STREAMTYPE_ATTACHMENT; break;

                default:
                    cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE,
                        "Unknown native stream type.");
                    return 1;
            }

            info->index = idx;
            retval = 0;
        }
    }

    return retval;

}