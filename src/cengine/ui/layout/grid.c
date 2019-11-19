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

        grid->ui_element = NULL;
        grid->transform = NULL;

        grid->bg_texture = NULL;

        grid->ui_elements = NULL;
    }

    return grid;

}

void ui_layout_grid_delete (void *grid_ptr) {

    if (grid_ptr) {
        GridLayout *grid = (GridLayout *) grid_ptr;

        grid->ui_element = NULL;
        ui_transform_component_delete (grid->transform);

        if (grid->bg_texture) SDL_DestroyTexture (grid->bg_texture);

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

// TODO: do we need a ui element?
// creates a new grid layout
// x and y for position, w and h for dimensions
GridLayout *ui_layout_grid_create (i32 x, i32 y, u32 w, u32 h) {

    GridLayout *grid = NULL;

    grid = ui_layout_grid_new ();
    grid->transform = ui_transform_component_create (x, y, w, h);

    return grid;

}

// adds a new element to the grid
// returns 0 on success, 1 if failed to add
u8 ui_layout_grid_add_element (GridLayout *grid, UIElement *ui_element) {

    u8 retval = 1;

    if (grid && ui_element) {
        if (grid->ui_elements) {
            // TODO: check for max elements
            // add element in next available idx -> at the end for now
            grid->ui_elements[grid->next_x][grid->next_y] = ui_element;

            // modify the element's absolutue posistion in the screen -> transform -> position
            // first get the position
            u32 x = grid->transform->rect.x;        // starting x
            u32 y = grid->transform->rect.y;        // starting y
            x += (grid->cell_width * grid->next_x);
            y += (grid->cell_height * grid->next_y);

            // get the middle of the cell
            // x += grid->cell_width / 2;
            // y += grid->cell_height / 2;

            // printf ("%d - %d\n", x, y);

            Image *image = (Image *) ui_element->element;
            // image->transform->rect.w = grid->cell_width;
            // image->transform->rect.h = grid->cell_height;

            UIRect cell = { .x = (grid->transform->rect.x + (grid->cell_width * grid->next_x)), 
                .y = (grid->transform->rect.y + (grid->cell_height * grid->next_y)),
                .w = grid->cell_width, .h = grid->cell_height };
            printf ("x: %d - y: %d - w: %d - h: %d\n", cell.x, cell.y, cell.w, cell.h);
            ui_transform_component_set_pos (image->transform, &cell, UI_POS_MIDDLE_CENTER, false);

            // TODO:
            // modify elemenst size based on grid constraints -> transform -> dimensions

            if (grid->next_x < (grid->cols - 1)) grid->next_x += 1;
            else {
                grid->next_x = 0;
                grid->next_y += 1;
            } 
        }
    }

    return retval;

}

void ui_layout_grid_remove_element (GridLayout *grid) {

    if (grid) {

    }

}