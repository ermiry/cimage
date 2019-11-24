#include <stdlib.h>
#include <string.h>

#include "cengine/types/types.h"

#include "cengine/renderer.h"

#include "cengine/ui/components/transform.h"
#include "cengine/ui/layout/grid.h"
#include "cengine/ui/image.h"

static GridLayout *ui_layout_grid_new (void) {

    GridLayout *grid = (GridLayout *) malloc (sizeof (GridLayout));
    if (grid) {
        memset (grid, 0, sizeof (GridLayout));

        grid->transform = NULL;

        grid->ui_elements = NULL;
    }

    return grid;

}

void ui_layout_grid_delete (void *grid_ptr) {

    if (grid_ptr) {
        GridLayout *grid = (GridLayout *) grid_ptr;

        ui_transform_component_delete (grid->transform);

        if (grid->ui_elements) {
            for (u32 i = 0; i < grid->cols; i++) 
                if (grid->ui_elements[i]) free (grid->ui_elements[i]);

            free (grid->ui_elements);
        }

        free (grid);
    }

}

// TODO: update layout
// sets the number of columns and rows available for the grid
void ui_layout_grid_set_grid (GridLayout *grid, u32 cols, u32 rows) {

    if (grid) {
        grid->cols = cols;
        grid->rows = rows;

        grid->ui_elements = (UIElement ***) calloc (grid->cols, sizeof (UIElement **));
        for (u32 i = 0; i < grid->cols; i++) 
            grid->ui_elements[i] = (UIElement **) calloc (grid->rows, sizeof (UIElement *));

        grid->max_n_ui_elements = grid->cols * grid->rows;
        grid->curr_n_ui_elements = 0;
        grid->next_x = grid->next_y = 0;

        grid->cell_width = grid->transform->rect.w / grid->cols;
        grid->cell_height = grid->transform->rect.h / grid->rows;
    }

}

// TODO: update layout
// sets the preferred size of the cells, the layout must be of the correct size
void ui_layout_grid_set_cell_size (GridLayout *grid, u32 width, u32 height) {

    if (grid) {
        grid->cell_width = width;
        grid->cell_height = height;
    }

}

// creates a new grid layout
// x and y for position, w and h for dimensions
GridLayout *ui_layout_grid_create (i32 x, i32 y, u32 w, u32 h) {

    GridLayout *grid = ui_layout_grid_new ();

    grid->transform = ui_transform_component_create (x, y, w, h);

    return grid;

}

// adds a new element to the grid
// returns 0 on success, 1 if failed to add
u8 ui_layout_grid_add_element (GridLayout *grid, UIElement *ui_element) {

    u8 retval = 1;

    if (grid && ui_element) {
        if (grid->ui_elements && (grid->curr_n_ui_elements < grid->max_n_ui_elements)) {
            // add element in next available idx -> at the end for now
            grid->ui_elements[grid->next_x][grid->next_y] = ui_element;

            Image *image = (Image *) ui_element->element;

            u32 max_width = grid->cell_width - 40;      // Max width for the image
            u32 max_height = grid->cell_height - 40;    // Max height for the image
            float ratio = 0;                            // Used for aspect ratio
            u32 width = image->transform->rect.w;       // Current image width
            u32 height = image->transform->rect.h;      // Current image height

            // Check if the current width is larger than the max
            if (width > max_width){
                ratio = (float) max_width / width;      // get ratio for scaling image
                image->transform->rect.w = max_width;   // Set new width
                image->transform->rect.h *= ratio;      // Scale height based on ratio
                height = height * ratio;                // Reset height to match scaled image
                width = width * ratio;                  // Reset width to match scaled image
            }

            // Check if current height is larger than max
            if (height > max_height){
                ratio = (float) max_height / height;    // get ratio for scaling image
                image->transform->rect.h = max_height;  // Set new height
                image->transform->rect.w *= ratio;      // Scale width based on ratio
                width = width * ratio;                  // Reset width to match scaled image
                height = height * ratio;                // Reset height to match scaled image
            }

            UIRect cell = { .x = (grid->transform->rect.x + (grid->cell_width * grid->next_x)), 
                .y = (grid->transform->rect.y + (grid->cell_height * grid->next_y)),
                .w = grid->cell_width, .h = grid->cell_height };
            // printf ("x: %d - y: %d - w: %d - h: %d\n", cell.x, cell.y, cell.w, cell.h);
            ui_transform_component_set_pos (image->transform, NULL, &cell, UI_POS_MIDDLE_CENTER, false);

            grid->curr_n_ui_elements += 1;

            if (grid->next_x < (grid->cols - 1)) grid->next_x += 1;
            else {
                grid->next_x = 0;
                if (grid->curr_n_ui_elements >= grid->max_n_ui_elements) grid->next_y = 0;
                else grid->next_y += 1;
            } 

            retval = 0;
        }
    }

    return retval;

}

// removes an element from the grid
void ui_layout_grid_remove_element (GridLayout *grid, UIElement *ui_element) {

    if (grid) {

    }

}