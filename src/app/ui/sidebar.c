#include "cengine/ui/ui.h"
#include "cengine/ui/panel.h"
#include "cengine/ui/button.h"

#include "app/ui/sidebar.h"

Panel *sidebar = NULL;
static Button *photos_button = NULL;
static Button *presentation_button = NULL;
static Button *settings_button = NULL;

void sidebar_init (u32 screen_height) {

    Renderer *main_renderer = renderer_get_by_name ("main");

    RGBA_Color blue_night = { 53, 59, 72, 255 };

    sidebar = ui_panel_create (0, 0, SIDEBAR_WIDTH, screen_height, UI_POS_LEFT_UPPER_CORNER, main_renderer);
    ui_panel_set_bg_colour (sidebar, main_renderer, blue_night);
    ui_element_set_layer (main_renderer->ui, sidebar->ui_element, "top");

    photos_button = ui_button_create (18, 18, 64, 64, UI_POS_LEFT_UPPER_CORNER, main_renderer);
    // ui_button_set_action (photos_button, NULL, NULL);
    // ui_button_set_ouline_colour (photos_button, RGBA_WHITE);
    ui_button_set_sprite (photos_button, main_renderer, BUTTON_STATE_MOUSE_OUT, "./assets/icons/gallery.png");
    ui_element_set_layer (main_renderer->ui, photos_button->ui_element, "top");

    presentation_button = ui_button_create (18, 118, 64, 64, UI_POS_LEFT_UPPER_CORNER, main_renderer);
    // ui_button_set_action (photos_button, NULL, NULL);
    // ui_button_set_ouline_colour (photos_button, RGBA_WHITE);
    ui_button_set_sprite (presentation_button, main_renderer, BUTTON_STATE_MOUSE_OUT, "./assets/icons/presentation.png");
    ui_element_set_layer (main_renderer->ui, presentation_button->ui_element, "top");

    settings_button = ui_button_create (18, -18, 64, 64, UI_POS_LEFT_BOTTOM_CORNER, main_renderer);
    // ui_button_set_action (settings_button, game_state_change, change_main_state_settings);
    // ui_button_set_ouline_colour (settings_button, RGBA_WHITE);
    ui_button_set_sprite (settings_button, main_renderer, BUTTON_STATE_MOUSE_OUT, "./assets/icons/settings.png");
    ui_element_set_layer (main_renderer->ui, settings_button->ui_element, "top");

}

void sidebar_end (void) {

    // if (sidebar) ui_element_destroy (sidebar->ui_element);

    // if (photos_button) ui_element_destroy (photos_button->ui_element);
    // if (presentation_button) ui_element_destroy (presentation_button->ui_element);
    // if (settings_button) ui_element_destroy (settings_button->ui_element);

}