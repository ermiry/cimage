#ifndef _STATE_APP_H_
#define _STATE_APP_H_

#include "cengine/collections/dlist.h"

#include "cengine/manager/state.h"

extern State *app_state;
extern State *app_state_new (void);

extern void images_folder_select (void *args);

#endif