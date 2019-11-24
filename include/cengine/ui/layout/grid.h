#ifndef _CENGINE_UI_LAYOUT_GRID_H_
#define _CENGINE_UI_LAYOUT_GRID_H_

#include "cengine/types/types.h"

#include "cengine/ui/ui.h"
#include "cengine/ui/components/transform.h"

typedef struct GridLayout {

    UITransform *transform;

    UITransform ***ui_elements_trans;
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

// sets the preferred size of the cells, the layout must be of the correct size
extern void ui_layout_grid_set_cell_size (GridLayout *grid, u32 width, u32 height);

// creates a new grid layout
// x and y for position, w and h for dimensions
extern GridLayout *ui_layout_grid_create (i32 x, i32 y, u32 w, u32 h);

// adds a new element to the grid
// returns 0 on success, 1 if failed to add
extern u8 ui_layout_grid_add_element (GridLayout *grid, UITransform *ui_element_trans);

// removes an element from the grid
extern void ui_layout_grid_remove_element (GridLayout *grid, UITransform *ui_element_trans);

#endif