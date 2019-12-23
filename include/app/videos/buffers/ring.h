#ifndef _CIMAGE_MEDIA_VIDEOS_RING_BUFFER_H_
#define _CIMAGE_MEDIA_VIDEOS_RING_BUFFER_H_

typedef struct RingBuffer {

	int size;
	int len;
	int wpos;
	int rpos;
	char* data;

} RingBuffer;

#endif