#include "cengine/types/types.h"

#include "cengine/renderer.h"

#include "cengine/ui/font.h"
#include "cengine/ui/panel.h"

static Panel *background_panel = NULL;

void splash_ui_init (void) {

    Renderer *main_renderer = renderer_get_by_name ("main");

    Font *font = ui_font_get_default ();

    u32 screen_width = main_renderer->window->window_size.width;
    u32 screen_height = main_renderer->window->window_size.height;

    /*** background ***/
    // RGBA_Color electromagnetic = { 47, 54, 64, 255 };
    RGBA_Color white = { 238, 238, 238, 255 };
    background_panel = ui_panel_create (0, 0, screen_width, screen_height, UI_POS_MIDDLE_CENTER, main_renderer);
    ui_panel_set_bg_colour (background_panel, main_renderer, white);
    ui_element_set_layer (main_renderer->ui, background_panel->ui_element, "back");

}

void splash_ui_end (void) {

    if (background_panel) ui_element_destroy (background_panel->ui_element);

}