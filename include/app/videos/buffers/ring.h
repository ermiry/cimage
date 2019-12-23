#ifndef _CIMAGE_MEDIA_VIDEOS_RING_BUFFER_H_
#define _CIMAGE_MEDIA_VIDEOS_RING_BUFFER_H_

typedef struct RingBuffer {

	int size;
	int len;
	int wpos;
	int rpos;
	char* data;

} RingBuffer;


extern RingBuffer *ring_buffer_create (unsigned int size);

extern void ring_buffer_destroy (RingBuffer* rb);

extern int ring_buffer_write (RingBuffer *rb, const char* data, int len);

extern int ring_buffer_read (RingBuffer *rb, char *data, int len);

extern int ring_buffer_peek (const RingBuffer *rb, char *data, int len);

extern int ring_buffer_advance (RingBuffer *rb, int len);

extern int ring_buffer_length (const RingBuffer *rb);

extern int ring_buffer_size (const RingBuffer *rb);

extern int ring_buffer_free_size (const RingBuffer *rb);

#endif