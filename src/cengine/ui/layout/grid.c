#include <stdlib.h>
#include <string.h>

#include "cengine/types/types.h"

#include "cengine/renderer.h"

#include "cengine/ui/layout/grid.h"
#include "cengine/ui/components/transform.h"

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