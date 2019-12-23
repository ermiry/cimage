#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_mutex.h>

#include "cengine/threads/thread.h"

#include "cengine/utils/utils.h"
#include "cengine/utils/log.h"

#include "app/videos/player.h"
#include "app/videos/decoder.h"

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

            // init video decoder

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
            // FIXME:
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
                // FIXME:
                // for (int i = 0; i < DECODER_COUNT; i++) Kit_CloseDecoder (player->decoders[i]);
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

        // FIXME:
        // Shutdown decoders
        // for (int i = 0; i < DECODER_COUNT; i++) Kit_CloseDecoder (player->decoders[i]);

        video_player_delete (player);
    }

}