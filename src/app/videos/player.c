#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_mutex.h>

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
        for (int i = 0; i < DECODER_COUNT; i++) Kit_CloseDecoder(player->decoders[i]);

        video_player_delete (player);
    }

}