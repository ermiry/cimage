#ifndef _CENGINE_UI_PANEL_H_
#define _CENGINE_UI_PANEL_H_

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

#include "cengine/types/types.h"

#include "cengine/ui/ui.h"
#include "cengine/ui/components/transform.h"

typedef struct Panel {

    UIElement *ui_element;

    UITransform *transform;
    RGBA_Color bgcolor;

    SDL_Rect texture_rect;
    SDL_Texture *texture;

} Panel;

extern void ui_panel_delete (void *panel_ptr);

// sets the background colour of the panel
extern void ui_panel_set_bg_colour (Panel *panel, RGBA_Color colour);

// creates a new panel
// x and y for position
extern Panel *panel_create (u32 x, u32 y, u32 w, u32 h, UIPosition pos);

// draws the panel to the screen
extern void ui_panel_draw (Panel *panel);

#endif