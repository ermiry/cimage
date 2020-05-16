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

// sets the background colour of the tooltip
extern void ui_tooltip_set_bg_colour (Tooltip *tooltip, Renderer *renderer, RGBA_Color colour);

// removes the background from the tooltip
extern void ui_tooltip_remove_background (Tooltip *tooltip);

// adds a new ui element to the tooltip's layout's in the specified position (0 indexed)
extern void ui_tooltip_add_element_at_pos (Tooltip *tooltip, UIElement *ui_element, u32 pos);

// adds a new ui element to the tooltip's layout's END
extern void ui_tooltip_add_element_at_end (Tooltip *tooltip, UIElement *ui_element);

// returns the ui element that is at the required position in the tooltip's layout
extern UIElement *ui_tooltip_get_element_at (Tooltip *tooltip, unsigned int pos);

// removes a ui element form the tooltip's layout
extern u8 ui_tooltip_remove_element (Tooltip *tooltip, UIElement *ui_element);

extern Tooltip *ui_tooltip_create (Renderer *renderer);

#endif