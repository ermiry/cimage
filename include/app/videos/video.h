#ifndef _CIMAGE_MEDIA_VIDEOS_VIDEO_H_
#define _CIMAGE_MEDIA_VIDEOS_VIDEO_H_

#include "app/videos/decoder.h"
#include "app/videos/source.h"

extern Decoder *video_create_decoder (const VideoSource *src, int stream_index);

#endif