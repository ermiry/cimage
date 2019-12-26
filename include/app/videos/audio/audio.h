#ifndef _CIMAGE_MEDIA_VIDEOS_AUDIO_H_
#define _CIMAGE_MEDIA_VIDEOS_AUDIO_H_

#include "app/videos/decoder.h"
#include "app/videos/source.h"

#define AUDIO_SYNC_THRESHOLD            0.05

extern Decoder *audio_create_decoder (const VideoSource *src, int stream_idx);

#endif