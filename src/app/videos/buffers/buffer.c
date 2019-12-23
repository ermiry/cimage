#include <stdlib.h>
#include <assert.h>

#include "app/videos/buffers/buffer.h"

Buffer *buffer_create (unsigned int size, BufferFreeCallback free_cb) {
    Buffer *b = calloc(1, sizeof(Buffer));
    if(b == NULL) {
        return NULL;
    }
    b->size = size;
    b->free_cb = free_cb;
    b->data = calloc(size, sizeof(void*));
    if(b->data == NULL) {
        free(b);
        return NULL;
    }
    return b;
}

unsigned int buffer_length (const Buffer *buffer) {
    return buffer->write_p - buffer->read_p;
}

void buffer_destroy (Buffer *buffer) {
    if(buffer == NULL) return;
    buffer_clear(buffer);
    free(buffer->data);
    free(buffer);
}

void buffer_clear (Buffer *buffer) {
    void *data;
    if(buffer->free_cb == NULL)
        return;
    while((data = buffer_read (buffer)) != NULL) {
        buffer->free_cb(data);
    }
}

void *buffer_read (Buffer *buffer) {
    assert(buffer != NULL);
    if(buffer->read_p < buffer->write_p) {
        void *out = buffer->data[buffer->read_p % buffer->size];
        buffer->data[buffer->read_p % buffer->size] = NULL;
        buffer->read_p++;
        if(buffer->read_p >= buffer->size) {
            buffer->read_p = buffer->read_p % buffer->size;
            buffer->write_p = buffer->write_p % buffer->size;
        }
        return out;
    }
    return NULL;
}

void *buffer_peek (const Buffer *buffer) {
    assert(buffer != NULL);
    if(buffer->read_p < buffer->write_p) {
        return buffer->data[buffer->read_p % buffer->size];
    }
    return NULL;
}

void buffer_advance (Buffer *buffer) {
    assert(buffer != NULL);
    if(buffer->read_p < buffer->write_p) {
        buffer->data[buffer->read_p % buffer->size] = NULL;
        buffer->read_p++;
        if(buffer->read_p >= buffer->size) {
            buffer->read_p = buffer->read_p % buffer->size;
            buffer->write_p = buffer->write_p % buffer->size;
        }
    }
}

void buffer_for_each_item (const Buffer *buffer, ForEachItemCallback cb, void *userdata) {
    unsigned int read_p = buffer->read_p;
    unsigned int write_p = buffer->write_p;
    while(read_p < write_p) {
        cb(buffer->data[read_p++ % buffer->size], userdata);
        if(read_p >= buffer->size) {
            read_p = read_p % buffer->size;
            write_p = write_p % buffer->size;
        }
    }
}

int buffer_write (Buffer *buffer, void *ptr) {
    assert(buffer != NULL);
    assert(ptr != NULL);

    if(!buffer_is_full (buffer)) {
        buffer->data[buffer->write_p % buffer->size] = ptr;
        buffer->write_p++;
        return 0;
    }
    return 1;
}

int buffer_is_full (const Buffer *buffer) {

    int len = buffer->write_p - buffer->read_p;
    int k = (len >= buffer->size);
    return k;
}
