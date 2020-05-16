#ifndef _CIMAGE_UI_SIDEBAR_H_
#define _CIMAGE_UI_SIDEBAR_H_

#include "cengine/types/types.h"

#include "cengine/ui/panel.h"

#define SIDEBAR_WIDTH           100

extern Panel *sidebar;

extern void sidebar_init (u32 screen_height);

extern void sidebar_end (void);

#endif