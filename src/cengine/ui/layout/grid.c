#include <stdlib.h>
#include <string.h>

#include "cengine/types/types.h"
#include "cengine/collections/dlist.h"

#include "cengine/renderer.h"

#include "cengine/ui/components/transform.h"
#include "cengine/ui/layout/grid.h"
#include "cengine/ui/image.h"

typedef struct GridElement {

    UITransform *trans;
    u32 x, y;

} GridElement;

static GridElement *grid_element_new (UITransform *trans, u32 x, u32 y) {

    GridElement *element = (GridElement *) malloc (sizeof (GridElement));
    if (element) {
        element->trans = trans;
        element->x = x;
        element->y = y;
    }

    return element;

}

static inline void grid_element_delete (void *grid_element_ptr) {

    if (grid_element_ptr) free (grid_element_ptr);

}

static GridLayout *ui_layout_grid_new (void) {

    GridLayout *grid = (GridLayout *) malloc (sizeof (GridLayout));
    if (grid) {
        memset (grid, 0, sizeof (GridLayout));

        grid->transform = NULL;
        grid->ui_transforms = NULL;
    }

    return grid;

}

void ui_layout_grid_delete (void *grid_ptr) {

    if (grid_ptr) {
        GridLayout *grid = (GridLayout *) grid_ptr;

        ui_transform_component_delete (grid->transform);

        dlist_delete (grid->ui_transforms);

        free (grid);
    }

}

// sets the number of columns and rows available for the grid
void ui_layout_grid_set_grid (GridLayout *grid, u32 cols, u32 rows) {

    if (grid) {
        grid->cols = cols;
        grid->rows = rows;

        grid->ui_transforms = dlist_init (grid_element_delete, NULL);

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

static void ui_layout_grid_update_element_size (GridLayout *grid, UITransform *ui_element_trans) {


    if (grid && ui_element_trans) {
        u32 max_width = grid->cell_width - 40;      // Max width for the image
        u32 max_height = grid->cell_height - 40;    // Max height for the image
        float ratio = 0;                            // Used for aspect ratio
        u32 width = ui_element_trans->rect.w;       // Current image width
        u32 height = ui_element_trans->rect.h;      // Current image height

        // Check if the current width is larger than the max
        if (width > max_width){
            ratio = (float) max_width / width;      // get ratio for scaling image
            ui_element_trans->rect.w = max_width;   // Set new width
            ui_element_trans->rect.h *= ratio;      // Scale height based on ratio
            height = height * ratio;                // Reset height to match scaled image
            width = width * ratio;                  // Reset width to match scaled image
        }

        // Check if current height is larger than max
        if (height > max_height){
            ratio = (float) max_height / height;    // get ratio for scaling image
            ui_element_trans->rect.h = max_height;  // Set new height
            ui_element_trans->rect.w *= ratio;      // Scale width based on ratio
            width = width * ratio;                  // Reset width to match scaled image
            height = height * ratio;                // Reset height to match scaled image
        }
    }

}

static void ui_layout_grid_update_element_pos (GridLayout *grid, UITransform *ui_element_trans) {

    if (grid && ui_element_trans) {
        UIRect cell = { .x = (grid->transform->rect.x + (grid->cell_width * grid->next_x)), 
            .y = (grid->transform->rect.y + (grid->cell_height * grid->next_y)),
            .w = grid->cell_width, .h = grid->cell_height };
        // printf ("x: %d - y: %d - w: %d - h: %d\n", cell.x, cell.y, cell.w, cell.h);
        ui_transform_component_set_pos (ui_element_trans, NULL, &cell, UI_POS_MIDDLE_CENTER, false);
    }

}

// updates the grid with a new size
// returns 0 on success update, 1 on failure
int ui_layout_grid_update_dimensions (GridLayout *grid, u32 cols, u32 rows) {

    int retval = 1;

    // if (grid) {
    //     if (grid->ui_elements_trans) {
    //         // allocate new grid
    //         UITransform ***ui_elements_trans = (UITransform ***) calloc (cols, sizeof (UITransform **));
    //         if (ui_elements_trans) {
    //             for (u32 i = 0; i < cols; i++)
    //                 ui_elements_trans[i] = (UITransform **) calloc (rows, sizeof (UITransform *));

    //             for (u32 i = 0; i < cols; i++) {
    //                 for (u32 j = 0; j < rows; j++) {
    //                     ui_elements_trans[i][j] = NULL;
    //                 }
    //             }

    //             // copy over past elements to new grid
    //             UITransform ***new_element = ui_elements_trans;
    //             UITransform ***old_element = grid->ui_elements_trans;

    //             //  while (*from) *to++ = *from++;

    //             // while (*old_element) *new_element++ = *old_element++;

    //             int test = 0;
    //             while (old_element) {
    //                 **new_element = **old_element;
    //                 new_element++;
    //                 old_element++;
    //                 test++;
    //             }
    //             printf ("test: %d\n", test);

    //             // size_t size = sizeof (UITransform *) * cols * rows;
    //             // memcpy (ui_elements_trans, grid->ui_elements_trans, size);

    //             // int test[3][2] = {{1,4},{2,5},{2,8}}, *p;
    //             // for(p = &test[0][0]; p <= &test[2][1]; p++)
    //             // {
    //             //     printf("%d\n", *p);
    //             // }

    //             grid->cols = cols;
    //             grid->rows = rows;
    //             grid->curr_n_ui_elements = 0;
    //             grid->next_x = 0;
    //             grid->next_y = 0;
    //             grid->max_n_ui_elements = grid->cols * grid->rows;
    //             grid->next_x = grid->next_y = 0;        // FIXME: update

    //             grid->cell_width = grid->transform->rect.w / grid->cols;
    //             grid->cell_height = grid->transform->rect.h / grid->rows;

    //             UITransform ***trans = ui_elements_trans;
    //             int idx = 0;
    //             while (idx < (cols * rows)) {
    //                 if (**trans) {
    //                     ui_layout_grid_update_element_size (grid, **trans);
    //                     ui_layout_grid_update_element_pos (grid, **trans);

    //                     grid->curr_n_ui_elements += 1;

    //                     if (grid->next_x < (grid->cols - 1)) grid->next_x += 1;
    //                     else {
    //                         grid->next_x = 0;
    //                         if (grid->curr_n_ui_elements >= grid->max_n_ui_elements) grid->next_y = 0;
    //                         else grid->next_y += 1;
    //                     } 
    //                 }
    //                 trans++;
    //                 idx++;
    //             }

    //             // update each new element position
    //             // int count = 0;
    //             // for (u32 i = 0; i < rows; i++) {
    //             //     for (u32 j = 0; j < cols; j++) {
    //             //         if (ui_elements_trans[j][i]) {
    //             //             UITransform *trans = ui_elements_trans[j][i];
    //             //             printf ("%d - %d\n", trans->rect.x, trans->rect.y);
    //             //             count++;
    //             //             ui_layout_grid_update_element_size (grid, trans);
    //             //             ui_layout_grid_update_element_pos (grid, trans);

    //             //             grid->curr_n_ui_elements += 1;

    //             //             if (grid->next_x < (grid->cols - 1)) grid->next_x += 1;
    //             //             else {
    //             //                 grid->next_x = 0;
    //             //                 if (grid->curr_n_ui_elements >= grid->max_n_ui_elements) grid->next_y = 0;
    //             //                 else grid->next_y += 1;
    //             //             } 
    //             //         }
    //             //     }
    //             // }

    //             // printf ("count: %d\n", count);

    //             for (u32 i = 0; i < grid->cols; i++) 
    //                 if (grid->ui_elements_trans[i]) free (grid->ui_elements_trans[i]);

    //             free (grid->ui_elements_trans);

    //             grid->ui_elements_trans = ui_elements_trans;
    //         }
    //     }
    // }

    return retval;

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
u8 ui_layout_grid_add_element (GridLayout *grid, UITransform *ui_element_trans) {

    u8 retval = 1;

    if (grid && ui_element_trans) {
        if (grid->ui_transforms && (grid->curr_n_ui_elements < grid->max_n_ui_elements)) {
            // add element in next available idx -> at the end for now
            GridElement *grid_element = grid_element_new (ui_element_trans, grid->next_x, grid->next_y);

            ui_layout_grid_update_element_size (grid, ui_element_trans);
            ui_layout_grid_update_element_pos (grid, ui_element_trans);

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
void ui_layout_grid_remove_element (GridLayout *grid, UITransform *ui_element_trans) {

    if (grid) {

    }

}