#ifndef _CIMAGE_MEDIA_VIDEOS_FORMAT_H_
#define _CIMAGE_MEDIA_VIDEOS_FORMAT_H_

typedef struct OutputFormat {

	unsigned int format; ///< SDL Format (SDL_PixelFormat if video/subtitle, SDL_AudioFormat if audio)
	int is_signed;       ///< Signedness, 1 = signed, 0 = unsigned (if audio)
	int bytes;           ///< Bytes per sample per channel (if audio)
	int samplerate;      ///< Sampling rate (if audio)
	int channels;        ///< Channels (if audio)
	int width;           ///< Width in pixels (if video)
	int height;          ///< Height in pixels (if video)

} OutputFormat;

#endif