#ifndef _CIMAGE_MEDIA_VIDEOS_AUDIO_H_
#define _CIMAGE_MEDIA_VIDEOS_AUDIO_H_

#include "app/videos/decoder.h"
#include "app/videos/source.h"

extern Decoder *auido_create_decoder (const VideoSource *src, int stream_idx);

#endif