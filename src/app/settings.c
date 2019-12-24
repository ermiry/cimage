#include <stdlib.h>
#include <string.h>

#include "cimage.h"
#include "app/settings.h"
#include "app/videos/settings.h"

Settings *main_settings = NULL;

static Settings *settings_new (void) {

    Settings *s = (Settings *) malloc (sizeof (Settings));
    if (s) {
        memset (s, 0, sizeof (Settings));
        s->video_settings = NULL;
    }

    return s;

}

void settings_delete (Settings *s) {

    if (s) {
		video_settings_delete (s->video_settings);
		free (s);
    }

}

Settings *settings_create (void) {

	Settings *s = settings_new ();
	if (s) {
		s->screen_width = DEFAULT_SCREEN_WIDTH;
		s->screen_height = DEFAULT_SCREEN_HEIGHT;
		s->fps_limit = FPS_LIMIT;

		s->video_settings = video_settings_create ();
	}

	return s;

}