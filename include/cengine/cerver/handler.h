#ifndef _CERVER_HANDLER_H_
#define _CERVER_HANDLER_H_

#include "cengine/types/types.h"

extern int client_poll_get_free_idx (Client *client);

extern u8 client_poll (void *data);

#endif