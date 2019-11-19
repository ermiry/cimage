#include <stdlib.h>

#include "cengine/types/types.h"
#include "cengine/types/string.h"

#include "cengine/ui/ui.h"
#include "cengine/ui/font.h"
#include "cengine/ui/textbox.h"
#include "cengine/ui/button.h"
#include "cengine/ui/image.h"
#include "cengine/ui/panel.h"

static Panel *background_panel = NULL;

void app_ui_init (void) {

    Font *font = ui_font_get_default ();

    u32 screen_width = main_renderer->window_size.width;
    u32 screen_height = main_renderer->window_size.height;

    /*** background ***/
    RGBA_Color electromagnetic = { 47, 54, 64, 255 };
    background_panel = ui_panel_create (0, 0, screen_width, screen_height, UI_POS_MIDDLE_CENTER);
    ui_panel_set_bg_colour (background_panel, electromagnetic);

}

void app_ui_end (void) {

    ui_element_destroy (background_panel->ui_element);

}