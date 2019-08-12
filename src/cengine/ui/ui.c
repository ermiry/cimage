#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "cengine/types/types.h"
#include "cengine/types/string.h"

#include "cengine/cengine.h"
#include "cengine/renderer.h"
#include "cengine/sprites.h"
#include "cengine/textures.h"

#include "cengine/ui/ui.h"
#include "cengine/ui/cursor.h"
#include "cengine/ui/font.h"
#include "cengine/ui/textbox.h"
#include "cengine/ui/image.h"
#include "cengine/ui/panel.h"
#include "cengine/ui/button.h"
#include "cengine/ui/inputfield.h"
#include "cengine/ui/check.h"
#include "cengine/ui/notification.h"

#include "cengine/utils/log.h"

/*** Common RGBA Colors ***/

RGBA_Color RGBA_NO_COLOR = { 0, 0, 0, 0 };
RGBA_Color RGBA_WHITE = { 255, 255, 255, 255 };
RGBA_Color RGBA_BLACK = { 0, 0, 0, 255 };
RGBA_Color RGBA_RED = { 255, 0, 0, 255 };
RGBA_Color RGBA_GREEN = { 0, 255, 0, 255 };
RGBA_Color RGBA_BLUE = { 0, 0, 255, 255 };

/*** Basic UI Elements ***/

#pragma region Basic UI Elements

UIRect ui_rect_create (u32 x, u32 y, u32 w, u32 h) {

    UIRect ret = { x, y, w, h };
    return ret;

}

UIRect ui_rect_union (UIRect a, UIRect b) {

    u32 x1 = MIN (a.x, b.x);
    u32 y1 = MIN (a.y, b.y);
    u32 x2 = MAX (a.x + a.w, b.x + b.w);
    u32 y2 = MAX (a.y + a.h, b.y + b.h);

    UIRect retval = { x1, y1, MAX (0, x2 - x1), MAX (0, y2 - y1) };
    return retval;

}

RGBA_Color ui_rgba_color_create (u8 r, u8 g, u8 b, u8 a) { 

    RGBA_Color retval = { r, g, b, a };
    return retval;

}

#pragma endregion

/*** ui elements ***/

#pragma region ui elements

UIElement **ui_elements = NULL;
static u32 max_ui_elements;
u32 curr_max_ui_elements;
static u32 new_ui_element_id;

static u8 ui_elements_realloc (void) {

    u32 new_max_ui_elements = curr_max_ui_elements * 2;

    ui_elements = realloc (ui_elements, new_max_ui_elements * sizeof (UIElement *));
    if (ui_elements) {
        max_ui_elements = new_max_ui_elements;
        return 0;
    }

    return 1;

}

// init our ui elements structures
static u8 ui_elements_init (void) {

    ui_elements = (UIElement **) calloc (DEFAULT_MAX_UI_ELEMENTS, sizeof (UIElement *));
    if (ui_elements) {
        for (u32 i = 0; i < DEFAULT_MAX_UI_ELEMENTS; i++) ui_elements[i] = NULL;

        max_ui_elements = DEFAULT_MAX_UI_ELEMENTS;
        curr_max_ui_elements = 0;
        new_ui_element_id = 0;

        return 0;
    }

    return 1;

}

static i32 ui_element_get_free_spot (void) {

    for (u32 i = 0; i < curr_max_ui_elements; i++) 
        if (ui_elements[i]->id == -1)
            return i;

    return -1;

}

// ui element constructor
UIElement *ui_element_new (UIElementType type) {

    UIElement *new_element = NULL;

    // first check if we have a reusable ui element
    i32 spot = ui_element_get_free_spot ();

    if (spot >= 0) {
        new_element = ui_elements[spot];
        new_element->id = spot;
        new_element->active = true;
        new_element->type = type;
        new_element->element = NULL;
    }

    else {
        if (new_ui_element_id >= max_ui_elements) ui_elements_realloc ();

        new_element = (UIElement *) malloc (sizeof (UIElement));
        if (new_element) {
            new_element->id = new_ui_element_id;
            new_element->type = type;
            new_element->active = true;
            new_element->element = NULL;
            ui_elements[new_element->id] = new_element;
            new_ui_element_id++;
            curr_max_ui_elements++;
        }
    }

    return new_element;

}

void ui_element_delete (UIElement *ui_element) {

    if (ui_element) {
        ui_element->id = -1;
        
        if (ui_element->element) {
            switch (ui_element->type) {
                case UI_TEXTBOX: ui_textbox_delete (ui_element->element); break;
                case UI_IMAGE: ui_image_delete (ui_element->element); break;
                case UI_PANEL: ui_panel_delete (ui_element->element); break;
                case UI_BUTTON: ui_button_delete (ui_element->element); break;
                case UI_INPUT: ui_input_field_delete (ui_element->element); break;
                case UI_CHECK: ui_check_delete (ui_element->element); break;
                case UI_NOTI_CENTER: ui_noti_center_delete (ui_element->element); break;

                default: break;
            }
        }

        free (ui_element);
    }

}

#pragma endregion

#pragma region default assets

static const String *ui_default_assets_path = NULL;

// sets the location of cengine's default ui assets
void ui_default_assets_set_path (const char *pathname) {

    str_delete ((String *) ui_default_assets_path);
    ui_default_assets_path = pathname ? str_new (pathname) : NULL;

}

static u8 ui_default_assets_load_checks (void) {

    u8 retval = 1;

    // TODO:

    return retval;

}

// loads cengine's default ui assets
u8 ui_default_assets_load (void) {

    u8 retval = 1;

    if (ui_default_assets_path) {
        // printf ("%s\n", ui_default_assets_path->str);

        retval = 0;
    }

    else {
        cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, 
            "Failed to load cengine's default assets - ui default assets path set to NULL!");
    }

    return retval;

}

#pragma endregion

#pragma region public

void ui_render (void) {

    for (u32 i = 0; i < curr_max_ui_elements; i++) {
        if (ui_elements[i]->active) {
            switch (ui_elements[i]->type) {
                case UI_TEXTBOX: {
                    ui_textbox_draw ((TextBox *) ui_elements[i]->element);
                }
                break;

                case UI_IMAGE: {
                    ui_image_draw ((Image *) ui_elements[i]->element);
                } break;

                case UI_PANEL: {
                    ui_panel_draw ((Panel *) ui_elements[i]->element);
                } break;

                case UI_BUTTON: {
                    ui_button_draw ((Button *) ui_elements[i]->element);
                }
                break;

                case UI_INPUT: {
                    ui_input_field_draw ((InputField *) ui_elements[i]->element);
                } break;

                case UI_CHECK: {
                    ui_check_draw ((Check *) ui_elements[i]->element);
                } break;

                case UI_NOTI_CENTER: {
                    ui_noti_center_draw ((NotiCenter *) ui_elements[i]->element);
                } break;

                default: break;
            }
        }
    }

    // render the cursor on top of everything
    ui_cursor_draw (main_cursor);

}

// init main ui elements
u8 ui_init (void) {

    int errors = 0;

    // init ui elements
    errors = ui_elements_init ();

    // init and load fonts
    errors = ui_fonts_init ();

    return errors;

}

// destroy main ui elements
u8 ui_destroy (void) {

    // ui elements
    if (ui_elements) {
        for (u32 i = 0; i < curr_max_ui_elements; i++)
            ui_element_delete (ui_elements[i]);

        free (ui_elements);
    }
    
    ui_cursor_delete (main_cursor);     // cursor

    ui_font_end ();     // fonts

    str_delete ((String *) ui_default_assets_path);

    #ifdef CENGINE_DEBUG
    cengine_log_msg (stdout, LOG_SUCCESS, LOG_NO_TYPE, "Done cleaning cengine ui.");
    #endif

    return 0;

}

#pragma endregion