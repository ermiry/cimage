#ifndef _CENGINE_UI_TOOLTIP_H_
#define _CENGINE_UI_TOOLTIP_H_

#include <stdbool.h>

#include <SDL2/SDL_rect.h>

#include "cengine/collections/dlist.h"

#include "cengine/renderer.h"

#include "cengine/ui/ui.h"
#include "cengine/ui/components/transform.h"

#include "cengine/ui/layout/layout.h"
#include "cengine/ui/layout/vertical.h"

typedef struct Tooltip {

    struct _Renderer *renderer;

    UIElement *ui_element;

    // background
	bool colour;
	RGBA_Color bg_colour;
	SDL_Texture *bg_texture;
	SDL_Rect bg_texture_rect;

    VerticalLayout *vertical;

    DoubleList *children;

} Tooltip;

extern void ui_tooltip_delete (void *tooltip_ptr);

#endif