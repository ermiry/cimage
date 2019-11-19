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

static Panel *sidebar = NULL;

static Button *photos_button = NULL;
static Button *settings_button = NULL;

// FIXME: set actions
static void sidebar_init (u32 screen_height) {

    RGBA_Color blue_night = { 53, 59, 72, 255 };

    sidebar = ui_panel_create (0, 0, 100, screen_height, UI_POS_LEFT_UPPER_CORNER);
    ui_panel_set_bg_colour (sidebar, blue_night);
    ui_element_set_layer (sidebar->ui_element, "top");

    photos_button = ui_button_create (18, 18, 64, 64, UI_POS_LEFT_UPPER_CORNER);
    // ui_button_set_action (photos_button, NULL, NULL);
    // ui_button_set_ouline_colour (photos_button, RGBA_WHITE);
    ui_button_set_sprite (photos_button, BUTTON_STATE_MOUSE_OUT, "./assets/icons/photos.png");
    ui_element_set_layer (photos_button->ui_element, "top");

    settings_button = ui_button_create (18, -18, 64, 64, UI_POS_LEFT_BOTTOM_CORNER);
    // ui_button_set_action (settings_button, game_state_change, change_main_state_settings);
    // ui_button_set_ouline_colour (settings_button, RGBA_WHITE);
    ui_button_set_sprite (settings_button, BUTTON_STATE_MOUSE_OUT, "./assets/icons/settings.png");
    ui_element_set_layer (settings_button->ui_element, "top");

}

static void sidebar_end (void) {

    if (sidebar) ui_element_destroy (sidebar->ui_element);

    if (photos_button) ui_element_destroy (photos_button->ui_element);
    if (settings_button) ui_element_destroy (settings_button->ui_element);

}

void app_ui_init (void) {

    Font *font = ui_font_get_default ();

    u32 screen_width = main_renderer->window_size.width;
    u32 screen_height = main_renderer->window_size.height;

    /*** background ***/
    RGBA_Color electromagnetic = { 47, 54, 64, 255 };
    background_panel = ui_panel_create (0, 0, screen_width, screen_height, UI_POS_MIDDLE_CENTER);
    ui_panel_set_bg_colour (background_panel, electromagnetic);

    sidebar_init (screen_height);

}

void app_ui_end (void) {

    sidebar_end ();

    ui_element_destroy (background_panel->ui_element);

}