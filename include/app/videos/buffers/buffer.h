#ifndef _CIMAGE_MEDIA_VIDEOS_BUFFER_H_
#define _CIMAGE_MEDIA_VIDEOS_BUFFER_H_

typedef struct Buffer Buffer;

typedef void (*BufferFreeCallback)(void*);
typedef void (*ForEachItemCallback)(void*, void *userdata);

struct Kit_Buffer {

    unsigned int read_p;
    unsigned int write_p;
    unsigned int size;
    BufferFreeCallback free_cb;
    void **data;
    
};

#endif