#ifndef _CIMAGE_MEDIA_VIDEOS_SETTINGS_H_
#define _CIMAGE_MEDIA_VIDEOS_SETTINGS_H_

#include "cengine/types/types.h"

typedef struct VideoSettings {

    u32 dec_thread_count;

	u32 audio_buf_frames;

} VideoSettings;

#endif