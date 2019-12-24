#ifndef _CIMAGE_MEDIA_VIDEOS_CODEC_H_
#define _CIMAGE_MEDIA_VIDEOS_CODEC_H_

#define CODEC_NAME_MAX          8
#define CODEC_DESC_MAX          48

typedef struct Codec {

    unsigned int threads; 				///< Currently enabled threads (For all decoders)
    char name[CODEC_NAME_MAX]; 			///< Codec short name, eg. "ogg" or "webm"
    char description[CODEC_DESC_MAX]; 	///< Codec longer, more descriptive name

} Codec;

#endif