#ifndef _CIMAGE_MEDIA_VIDEOS_SETTINGS_H_
#define _CIMAGE_MEDIA_VIDEOS_SETTINGS_H_

#include "cengine/types/types.h"

typedef struct VideoSettings {

    u32 dec_thread_count;

	u32 video_buf_frames;
	u32 audio_buf_frames;

} VideoSettings;

extern void video_settings_delete (VideoSettings *s);

// creates a new video settings with default values
extern VideoSettings *video_settings_create (void);

#endif