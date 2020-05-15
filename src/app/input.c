#include "cengine/ui/layout/grid.h"
#include "cengine/ui/panel.h"

#include "app/ui/app.h"

int zoom_level = 0;

void zoom_more (void *args) {

    // get window with keyboard focus
    Window *window = NULL;
    for (ListElement *le = dlist_start (windows); le; le = le->next) {
        window = (Window *) le->data;
        if (window->keyboard) break;
    }

    if (!strcmp (window->renderer->name->str, "main")) {
        if (images_panel) {
            GridLayout *grid =  (GridLayout *) images_panel->layout;

            // FIXME: 14/05/2020 -- scroll not working with (grid->cols - 1) > 1
            if ((grid->cols - 1) > 2) {
                Renderer *main_renderer = renderer_get_by_name ("main");

                u32 window_width = main_renderer->window->window_size.width;
                u32 window_height = main_renderer->window->window_size.height;

                zoom_level -= 1;

                u32 cols = (grid->cols - 1);
                u32 rows = 4 + zoom_level;
                // printf ("\n\ncols %d - rows %d\n\n", cols, rows);

                u32 cell_width = (window_width - 100) / cols;
                u32 cell_height = (window_height / rows);

                ui_layout_grid_update_size (grid, cols, cell_width, cell_height);

                app_ui_statusbar_zoom_set (zoom_level);

                // printf ("+\n");
            }
        }
    }

}

void zoom_less (void *args) {

    // get window with keyboard focus
    Window *window = NULL;
    for (ListElement *le = dlist_start (windows); le; le = le->next) {
        window = (Window *) le->data;
        if (window->keyboard) break;
    }

    if (!strcmp (window->renderer->name->str, "main")) {
        if (images_panel) {
            GridLayout *grid =  (GridLayout *) images_panel->layout;

            if ((grid->cols + 1) < 20) {
                Renderer *main_renderer = renderer_get_by_name ("main");

                u32 window_width = main_renderer->window->window_size.width;
                u32 window_height = main_renderer->window->window_size.height;
                
                zoom_level += 1;

                u32 cols = (grid->cols + 1);
                u32 rows = 4 + zoom_level;
                // printf ("\n\ncols %d - rows %d\n\n", cols, rows);

                u32 cell_width = (window_width - 100) / cols;
                u32 cell_height = (window_height / rows);

                ui_layout_grid_update_size (grid, cols, cell_width, cell_height);

                app_ui_statusbar_zoom_set (zoom_level);

                // printf ("-\n");
            }
        }
    }

}