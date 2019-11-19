#ifndef _CENGINE_UI_LAYOUT_GRID_H_
#define _CENGINE_UI_LAYOUT_GRID_H_

#include "cengine/types/types.h"

#include "cengine/ui/ui.h"
#include "cengine/ui/components/transform.h"

typedef struct GridLayout {

    UIElement *ui_element;
    UITransform *transform;

    bool outline;
    RGBA_Color outline_colour;

    bool colour;
    RGBA_Color bg_colour;
    SDL_Texture *bg_texture;
    SDL_Rect bg_texture_rect;

    UIElement ***ui_elements;
    u32 n_ui_elements;

} GridLayout;

#endif