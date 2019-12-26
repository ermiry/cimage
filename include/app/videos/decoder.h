#ifndef _CIMAGE_MEDIA_VIDEOS_DECODER_H_
#define _CIMAGE_MEDIA_VIDEOS_DECODER_H_

#include <stdbool.h>

#include <SDL2/SDL_mutex.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

#include "app/videos/format.h"
#include "app/videos/source.h"
#include "app/videos/codec.h"
#include "app/videos/buffers/buffer.h"

enum {
	DEC_BUF_IN = 0,
	DEC_BUF_OUT,
	DEC_BUF_COUNT
};

enum DecoderIndex {

	VIDEO_DEC           = 0,
	AUDIO_DEC           = 1,
	SUBTITLE_DEC        = 2,
	DECODER_COUNT       = 3

};

typedef struct Decoder Decoder;

typedef int (*dec_decode_cb)(Decoder *dec, AVPacket *in_packet);
typedef void (*dec_close_cb)(Decoder *dec);
typedef void (*dec_free_packet_cb)(void *packet);

struct Decoder {

	int stream_index;            ///< Source stream index for the current stream
	double clock_sync;           ///< Sync source for current stream
	double clock_pos;            ///< Current pts for the stream
	OutputFormat output;        ///< Output format for the decoder

	AVCodecContext *codec_ctx;   ///< FFMpeg internal: Codec context
	AVFormatContext *format_ctx; ///< FFMpeg internal: Format context (owner: Kit_Source)

	SDL_mutex *output_lock;      ///< Threading lock for output buffer
	Buffer *buffer[2];          ///< Buffers for incoming and decoded packets

	void *userdata;              ///< Decoder specific information (Audio, video, subtitle context)
	dec_decode_cb dec_decode;    ///< Decoder decoding function callback
	dec_close_cb dec_close;      ///< Decoder close function callback

};

extern Decoder *decoder_create (const VideoSource *src, int stream_index, 
    int out_b_size, dec_free_packet_cb free_out_cb, int thread_count);

extern void decoder_close (Decoder *dec);

/*** info ***/

// returns 0 on success, 1 on error
extern int decoder_get_codec_info (const Decoder *dec, Codec *codec);

// returns 0 on success, 1 on error
extern int decoder_get_output_format (const Decoder *dec, OutputFormat *output);

extern int decoder_get_stream_index (const Decoder *dec);

/*** clock ***/

extern void decoder_set_clock_sync (Decoder *dec, double sync);

extern void decoder_change_clock_sync (Decoder *dec, double sync);

/*** output buffer ***/

extern int decoder_write_output (Decoder *dec, void *packet);

extern bool decoder_can_write_output (Decoder *dec);

#endif