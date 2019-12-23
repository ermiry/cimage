#include <stdlib.h>
#include <string.h>

#include "app/videos/buffers/buffer.h"

static Buffer *buffer_new (void) {

    Buffer *buffer = (Buffer *) malloc (sizeof (Buffer));
    if (buffer) {
        memset (buffer, 0, sizeof (Buffer));

        buffer->free_cb = NULL;
        buffer->data = NULL;
    }

    return buffer;

}

Buffer *buffer_create (unsigned int size, BufferFreeCallback free_cb) {

    Buffer *b = buffer_new ();
    if (b) {
        b->size = size;
        b->free_cb = free_cb;
        b->data = calloc(size, sizeof(void*));
        if(b->data == NULL) {
            free(b);
            return NULL;
        }
    }
    
    return b;
}

unsigned int buffer_length (const Buffer *buffer) {

    if (buffer) return buffer->write_p - buffer->read_p;
    else return 0;

}

void buffer_destroy (Buffer *buffer) {
    
    if (buffer) {
        buffer_clear (buffer);
        free (buffer->data);
        free (buffer);
    }

}

void buffer_clear (Buffer *buffer) {

    if (buffer) {
        void *data;
        if(buffer->free_cb == NULL)
            return;
        while((data = buffer_read (buffer)) != NULL) {
            buffer->free_cb(data);
        }
    }
    
}

void *buffer_read (Buffer *buffer) {

    if (buffer) {
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
    }
    
    return NULL;
}

void *buffer_peek (const Buffer *buffer) {

    if (buffer) {
        if(buffer->read_p < buffer->write_p) {
            return buffer->data[buffer->read_p % buffer->size];
        }
    }
    
    return NULL;
}

void buffer_advance (Buffer *buffer) {

    if (buffer) {
        if(buffer->read_p < buffer->write_p) {
            buffer->data[buffer->read_p % buffer->size] = NULL;
            buffer->read_p++;
            if(buffer->read_p >= buffer->size) {
                buffer->read_p = buffer->read_p % buffer->size;
                buffer->write_p = buffer->write_p % buffer->size;
            }
        }
    }
    
}

void buffer_for_each_item (const Buffer *buffer, ForEachItemCallback cb, void *userdata) {

    if (buffer) {
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
    
}

int buffer_write (Buffer *buffer, void *ptr) {

    if (buffer && ptr) {
         if(!buffer_is_full (buffer)) {
            buffer->data[buffer->write_p % buffer->size] = ptr;
            buffer->write_p++;
            return 0;
        }
        return 1;
    }
   
}

int buffer_is_full (const Buffer *buffer) {

    if (buffer) {
        int len = buffer->write_p - buffer->read_p;
        int k = (len >= buffer->size);
        return k;
    }

}