#ifndef _CIMAGE_SETTINGS_H_
#define _CIMAGE_SETTINGS_H_

#include "cengine/types/types.h"

#include "app/videos/settings.h"

typedef struct Settings {

	u32 screen_width;
	u32 screen_height;
	u32 fps_limit;

	VideoSettings *video_settings;

} Settings;

extern Settings *main_settings;

extern void settings_delete (Settings *s);

extern Settings *settings_create (void);

#endif