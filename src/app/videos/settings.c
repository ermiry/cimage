#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL_cpuinfo.h>

#include "app/videos/settings.h"

#define DEFAULT_DEC_THREAD_COUNT			4
#define DEFAULT_VIDEO_BUF_FRAMES			3
#define DEFAULT_AUDIO_BUF_FRAMES			64

static VideoSettings *video_settings_new (void) {

    VideoSettings *s = (VideoSettings *) malloc (sizeof (VideoSettings));
    if (s) memset (s, 0, sizeof (VideoSettings));
    return s;

}

void video_settings_delete (VideoSettings *s) {

    if (s) free (s);

}

// creates a new video settings with default values
VideoSettings *video_settings_create (void) {

    VideoSettings *s = video_settings_new ();
	if (s) {
		s->dec_thread_count = SDL_GetCPUCount () <= DEFAULT_DEC_THREAD_COUNT ? SDL_GetCPUCount () : DEFAULT_DEC_THREAD_COUNT;
		s->video_buf_frames = DEFAULT_VIDEO_BUF_FRAMES;
		s->audio_buf_frames = DEFAULT_AUDIO_BUF_FRAMES;
	}

	return s;

}