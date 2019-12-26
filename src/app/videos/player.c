#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_mutex.h>

#include "cengine/threads/thread.h"

#include "cengine/utils/utils.h"
#include "cengine/utils/log.h"

#include "app/videos/player.h"
#include "app/videos/decoder.h"
#include "app/videos/audio/audio.h"
#include "app/videos/video.h"

static VideoPlayer *video_player_new (void) {

    VideoPlayer *player = (VideoPlayer *) malloc (sizeof (VideoPlayer));
    if (player) {
        memset (player, 0, sizeof (VideoPlayer));

        for (unsigned int i = 0; i < 3; i++) player->decoders[i] = NULL;

        player->dec_thread = NULL;
        player->dec_lock = NULL;
        player->src = NULL;
    }

}

static void video_player_delete (VideoPlayer *player) {

    if (player) {
        free (player);
    }

}

VideoPlayer *video_player_create (const VideoSource *src,
                                    int video_stream_index,
                                    int audio_stream_index,
                                    int subtitle_stream_index,
                                    int screen_w,
                                    int screen_h) {

    VideoPlayer *player = NULL;

    if (src && (screen_w > 0) && (screen_h > 0)) {
        if (video_stream_index < 0 && subtitle_stream_index >= 0) {
            cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, 
                "Subtitle stream selected without video stream");
            return NULL;
        }

        player = video_player_new ();
        if (player) {
            bool error = false;

            // init audio decoder
			// TODO: add error handling
            player->decoders[AUDIO_DEC] = audio_create_decoder (src, audio_stream_index);

            // init video decoder
			player->decoders[VIDEO_DEC] = video_create_decoder (src, video_stream_index);

            // init subtitle decoder

            // create decoder thread lock
            player->dec_lock = SDL_CreateMutex ();
            if (!player->dec_lock) {
                cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, 
                    "Failed to create decoder thread lock mutex!");
                #ifdef CIMAGE_DEBUG
                cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, 
                    SDL_GetError ());
                #endif
                error = true;
            }

            // create decoder thread
            player->dec_thread = SDL_CreateThread (NULL, "decoder", player);
            if (!player->dec_thread) {
                cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, 
                    "Failed to create decoder thread!");
                #ifdef CIMAGE_DEBUG
                cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, SDL_GetError ());
                #endif
                error = true;
            }

            if (error) {
                if (player->dec_lock) SDL_DestroyMutex (player->dec_lock);
				for (int i = 0; i < DECODER_COUNT; i++) decoder_close (player->decoders[i]);
                video_player_delete (player);
                player = NULL;
            }

            else player->src = src;      // success!!
        }
    }

    return player;

}

void video_player_close (VideoPlayer *player) {

    if (player) {
        // Kill the decoder thread and mutex
        if (SDL_LockMutex (player->dec_lock) == 0) {
            player->state = PLAYER_CLOSED;
            SDL_UnlockMutex (player->dec_lock);
        }

        SDL_WaitThread (player->dec_thread, NULL);
        SDL_DestroyMutex (player->dec_lock);

        // Shutdown decoders
        for (int i = 0; i < DECODER_COUNT; i++) decoder_close (player->decoders[i]);

        video_player_delete (player);
    }

}

#pragma region getters

int video_player_get_video_stream (const VideoPlayer *player) {

    if (player) return decoder_get_stream_index (player->decoders[VIDEO_DEC]);
	return -1;

}

int video_player_get_audio_stream (const VideoPlayer *player) {

    if (player) return decoder_get_stream_index (player->decoders[AUDIO_DEC]);
	return -1;

}

int video_player_get_subtitle_stream (const VideoPlayer *player) {

    if (player) return decoder_get_stream_index (player->decoders[SUBTITLE_DEC]);
	return -1;

}

void video_player_get_info (const VideoPlayer *player, VideoPlayerInfo *info) {

    if (player && info) {
		void *streams[] = {&info->video, &info->audio, &info->subtitle};
		for (int i = 0; i < DECODER_COUNT; i++) {
			Decoder *dec = player->decoders[i];
			VideoPlayerStreamInfo *stream = streams[i];
			decoder_get_codec_info (dec, &stream->codec);
			decoder_get_output_format (dec, &stream->output);
		}
    }

}

PlayerState video_player_get_state (const VideoPlayer *player) {

	if (player) return player->state;
	return 0; 

}

double video_player_get_duration (const VideoPlayer *player) {

	if (player) {
		AVFormatContext *fmt_ctx = player->src->format_ctx;
		return (fmt_ctx->duration / AV_TIME_BASE);
	}

	return 0;
    
}

double video_player_get_position (const VideoPlayer *player) {

	if (player) {
		if (player->decoders[VIDEO_DEC]) {
			return ((Decoder*) player->decoders[VIDEO_DEC])->clock_pos;
		}
		if (player->decoders[AUDIO_DEC]) {
			return ((Decoder*) player->decoders[AUDIO_DEC])->clock_pos;
		}
	}

    return 0;
}

#pragma endregion