#ifndef _CIMAGE_MEDIA_VIDEOS_PLAYER_H_
#define _CIMAGE_MEDIA_VIDEOS_PLAYER_H_

#include "app/videos/source.h"
#include "app/videos/codec.h"
#include "app/videos/format.h"

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

typedef struct VideoPlayerStreamInfo {

	Codec codec;                ///< Decoder codec information
	OutputFormat output;        ///< Information about the output format

} VideoPlayerStreamInfo;

typedef struct VideoPlayerInfo {

	VideoPlayerStreamInfo video;        ///< Video stream data
	VideoPlayerStreamInfo audio;        ///< Audio stream data
	VideoPlayerStreamInfo subtitle;     ///< Subtitle stream data

} VideoPlayerInfo;

extern VideoPlayer *video_player_create (const VideoSource *src,
                                    int video_stream_index,
                                    int audio_stream_index,
                                    int subtitle_stream_index,
                                    int screen_w,
                                    int screen_h);

extern void video_player_close (VideoPlayer *player);

extern int video_player_get_video_stream (const VideoPlayer *player);

extern int video_player_get_audio_stream (const VideoPlayer *player);

extern int video_player_get_subtitle_stream (const VideoPlayer *player);

extern void video_player_get_info (const VideoPlayer *player, VideoPlayerInfo *info);

extern PlayerState video_player_get_state (const VideoPlayer *player);

extern double video_player_get_duration (const VideoPlayer *player);

extern double video_player_get_position (const VideoPlayer *player);

/*** main ***/

extern void video_player_start (VideoPlayer *player);

extern void video_player_stop (VideoPlayer *player);

extern void video_player_pause (VideoPlayer *player);

#endif