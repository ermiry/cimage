#include "cengine/types/types.h"

#include "cengine/renderer.h"

#include "cengine/ui/font.h"
#include "cengine/ui/panel.h"
#include "cengine/ui/image.h"
#include "cengine/ui/textbox.h"

#include "version.h"

static Panel *background_panel = NULL;
static Image *logo = NULL;
static TextBox *version_text = NULL;

void splash_ui_init (void) {

    Renderer *main_renderer = renderer_get_by_name ("main");

    Font *font = ui_font_get_default ();

    u32 screen_width = main_renderer->window->window_size.width;
    u32 screen_height = main_renderer->window->window_size.height;

    /*** background ***/
    RGBA_Color electromagnetic = { 47, 54, 64, 255 };
    RGBA_Color white = { 238, 238, 238, 255 };
    background_panel = ui_panel_create (0, 0, screen_width, screen_height, UI_POS_MIDDLE_CENTER, main_renderer);
    ui_panel_set_bg_colour (background_panel, main_renderer, white);
    ui_element_set_layer (main_renderer->ui, background_panel->ui_element, "back");

    /*** logo ***/
    logo = ui_image_create (0, 0, 0, 0, main_renderer);
    ui_image_set_sprite (logo, main_renderer, "assets/cimage-512.png");
    ui_image_set_pos (logo, NULL, UI_POS_MIDDLE_CENTER, main_renderer);
    logo->ui_element->transform->rect.y -= 100;

    /*** version ***/
    version_text = ui_textbox_create (0, 0, 500, 50, UI_POS_FREE, main_renderer);
    ui_textbox_set_pos (version_text, NULL, UI_POS_MIDDLE_CENTER, main_renderer);
    version_text->ui_element->transform->rect.y += 256;
    ui_textbox_set_text (version_text, main_renderer, CIMAGE_VERSION_NAME, font, 32, electromagnetic, false);
    ui_textbox_set_text_pos (version_text, UI_POS_MIDDLE_CENTER);
    // ui_textbox_set_ouline_colour (version_text, electromagnetic);

}

void splash_ui_end (void) {

    if (version_text) ui_element_destroy (version_text->ui_element);

    if (logo) ui_element_destroy (logo->ui_element);

    if (background_panel) ui_element_destroy (background_panel->ui_element);

}