#include <stdlib.h>

#include "cengine/renderer.h"

#include "cengine/ui/ui.h"
#include "cengine/ui/font.h"
#include "cengine/ui/panel.h"
#include "cengine/ui/textbox.h"

#include "cengine/utils/utils.h"

#include "app/ui/sidebar.h"

static Panel *settings_name_panel = NULL;
static TextBox *settings_name_text = NULL;

void settings_ui_init (void) {

	Renderer *renderer = renderer_get_by_name ("main");
	Font *font = ui_font_get_default ();

	u32 screen_width = renderer->window->window_size.width;
	u32 screen_height = renderer->window->window_size.height;

	RGBA_Color blue_horizon = { 75, 101, 132, 255 };
	settings_name_panel = ui_panel_create (0, 0, screen_width, 100, UI_POS_UPPER_CENTER, renderer);
	ui_panel_set_bg_colour (settings_name_panel, renderer, blue_horizon);

	settings_name_text = ui_textbox_create (SIDEBAR_WIDTH + 30, 10, 800, 80, UI_POS_LEFT_UPPER_CORNER, renderer);
	ui_textbox_set_text (settings_name_text, renderer, "Settings", font, 64, RGBA_WHITE, false);
	ui_textbox_set_text_pos (settings_name_text, UI_POS_LEFT_CENTER);
	// ui_textbox_set_ouline_colour (settings_name_text, RGBA_WHITE);

	settings_name_panel->ui_element->active = false;
	settings_name_text->ui_element->active = false;

}

void settings_ui_show (void *args) {

	settings_name_panel->ui_element->active = true;
	settings_name_text->ui_element->active = true;

}

void settings_ui_hide (void *args) {

	settings_name_panel->ui_element->active = false;
	settings_name_text->ui_element->active = false;

}

void settings_ui_end (void) {

	if (settings_name_panel) ui_element_destroy (settings_name_panel->ui_element);
	if (settings_name_text) ui_element_destroy (settings_name_text->ui_element);

}