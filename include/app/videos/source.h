#ifndef _CIMAGE_MEDIA_VIDEOS_SOURCE_H_
#define _CIMAGE_MEDIA_VIDEOS_SOURCE_H_

typedef enum StreamType {

    STREAMTYPE_UNKNOWN,         ///< Unknown stream type
    STREAMTYPE_VIDEO,           ///< Video stream
    STREAMTYPE_AUDIO,           ///< Audio stream
    STREAMTYPE_DATA,            ///< Data stream
    STREAMTYPE_SUBTITLE,        ///< Subtitle streawm
    STREAMTYPE_ATTACHMENT       ///< Attachment stream (images, etc)

} StreamType;

typedef struct VideoSource {

    void *format_ctx;           ///< FFmpeg: Videostream format context
    void *avio_ctx;             ///< FFmpeg: AVIO context

} VideoSource;

#endif