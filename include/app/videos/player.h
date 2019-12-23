#ifndef _CIMAGE_MEDIA_VIDEOS_PLAYER_H_
#define _CIMAGE_MEDIA_VIDEOS_PLAYER_H_

#include "app/videos/source.h"

typedef enum PlayerState {

    PLAYER_STOPPED = 0,     ///< Playback stopped or has not started yet.
    PLAYER_PLAYING,         ///< Playback started & player is actively decoding.
    PLAYER_PAUSED,          ///< Playback paused; player is actively decoding but no new data is given out.
    PLAYER_CLOSED,          ///< Playback is stopped and player is closing.

} PlayerState;

typedef struct VideoPlayer {

    PlayerState state;          ///< Playback state
    void *decoders[3];          ///< Decoder contexts
    void *dec_thread;           ///< Decoder thread
    void *dec_lock;             ///< Decoder lock
    const VideoSource *src;     ///< Reference to Audio/Video source
    double pause_started;       ///< Temporary flag for handling pauses

} VideoPlayer;

extern VideoPlayer *video_player_create (const VideoSource *src,
                                    int video_stream_index,
                                    int audio_stream_index,
                                    int subtitle_stream_index,
                                    int screen_w,
                                    int screen_h);

extern void video_player_close (VideoPlayer *player);

#endif