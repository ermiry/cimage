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
    u32 cols, rows;
    u32 max_n_ui_elements;
    u32 curr_n_ui_elements;
    u32 next_x, next_y;

    u32 cell_width;
    u32 cell_height;

} GridLayout;

extern void ui_layout_grid_delete (void *grid_ptr);

// sets the number of columns and rows available for the grid
extern void ui_layout_grid_set_grid (GridLayout *grid, u32 cols, u32 rows);

// creates a new grid layout
// x and y for position, w and h for dimensions
extern GridLayout *ui_layout_grid_create (i32 x, i32 y, u32 w, u32 h);

// adds a new element to the grid
// returns 0 on success, 1 if failed to add
extern u8 ui_layout_grid_add_element (GridLayout *grid, UIElement *ui_element);

#endif