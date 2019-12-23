#ifndef _CIMAGE_MEDIA_VIDEOS_BUFFER_H_
#define _CIMAGE_MEDIA_VIDEOS_BUFFER_H_

typedef struct Buffer Buffer;

typedef void (*BufferFreeCallback)(void*);
typedef void (*ForEachItemCallback)(void*, void *userdata);

struct Buffer {

    unsigned int read_p;
    unsigned int write_p;
    unsigned int size;
    BufferFreeCallback free_cb;
    void **data;
    
};

extern Buffer *buffer_create (unsigned int size, BufferFreeCallback free_cb);

extern unsigned int buffer_length (const Buffer *buffer);

extern void buffer_destroy (Buffer *buffer);

extern void buffer_clear (Buffer *buffer);

extern void *buffer_read (Buffer *buffer);

extern void *buffer_peek (const Buffer *buffer);

extern void buffer_advance (Buffer *buffer);

extern void buffer_for_each_item (const Buffer *buffer, ForEachItemCallback cb, void *userdata);

extern int buffer_write (Buffer *buffer, void *ptr);

extern int buffer_is_full (const Buffer *buffer);

#endif