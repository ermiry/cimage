#include <stdlib.h>
#include <string.h>

#include "app/videos/buffers/ring.h"

static RingBuffer *ring_buffer_new (void) {

    RingBuffer *rb = (RingBuffer *) malloc (sizeof (RingBuffer));
    if (rb) {
        memset (rb, 0, sizeof (RingBuffer));

        rb->data = NULL;
    }

    return rb;

}

RingBuffer *ring_buffer_create (unsigned int size) {
    
    RingBuffer *rb = ring_buffer_new ();
    if (rb) {
        rb->size = size;
        rb->data = malloc(size);
        if(rb->data == NULL) {
            free(rb);
            return NULL;
        }
    }

    return rb;
}


void ring_buffer_destroy (RingBuffer* rb) {

    if (rb) {
        free(rb->data);
        free(rb);
    }

}

int ring_buffer_write (RingBuffer *rb, const char* data, int len) {

    if (rb) {
        int k;
        len = (len > (rb->size - rb->len)) ? (rb->size - rb->len) : len;
        if(rb->len < rb->size) {
            if(len + rb->wpos > rb->size) {
                k = (len + rb->wpos) % rb->size;
                memcpy((rb->data + rb->wpos), data, len - k);
                memcpy(rb->data, data+(len-k), k);
            } else {
                memcpy((rb->data + rb->wpos), data, len);
            }
            rb->len += len;
            rb->wpos += len;
            if(rb->wpos >= rb->size) {
                rb->wpos = rb->wpos % rb->size;
            }
            return len;
        }
    }
    
    return 0;
}

static void ring_buffer_reading_data (const RingBuffer *rb, char *data, const int len) {
    
    if (rb) {
        int k;
        if(len + rb->rpos > rb->size) {
            k = (len + rb->rpos) % rb->size;
            memcpy(data, rb->data + rb->rpos, len - k);
            memcpy(data + (len - k), rb->data, k);
        } else {
            memcpy(data, rb->data + rb->rpos, len);
        }
    }
    
}

int ring_buffer_read (RingBuffer *rb, char *data, int len) {

    if (rb) {
        len = (len > rb->len) ? rb->len : len;
        if(rb->len > 0) {
            ring_buffer_reading_data(rb, data, len);
            rb->len -= len;
            rb->rpos += len;
            if(rb->rpos >= rb->size) {
                rb->rpos = rb->rpos % rb->size;
            }
            return len;
        }
    }
    
    return 0;
}

int ring_buffer_peek (const RingBuffer *rb, char *data, int len) {

    if (rb) {
        len = (len > rb->len) ? rb->len : len;
        if(rb->len > 0) {
            ring_buffer_reading_data(rb, data, len);
            return len;
        }
    }
    
    return 0;
}

int ring_buffer_advance (RingBuffer *rb, int len) {

    if (rb) {
        len = (len > rb->len) ? rb->len : len;
        if(rb->len > 0) {
            rb->len -= len;
            rb->rpos += len;
            if(rb->rpos >= rb->size) {
                rb->rpos = rb->rpos % rb->size;
            }
            return len;
        }
    }
    
    return 0;
}

int ring_buffer_length (const RingBuffer *rb) { if (rb) return rb->len; }

int ring_buffer_size (const RingBuffer *rb) { if (rb) return rb->size; }

int ring_buffer_free_size (const RingBuffer *rb) { if (rb) return rb->size - rb->len; }