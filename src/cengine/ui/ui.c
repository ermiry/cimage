#include <stdlib.h>
#include <stdbool.h>

#include "cengine/types/types.h"
#include "cengine/types/string.h"

#include "cengine/collections/dlist.h"

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
#include "cengine/ui/dropdown.h"

#include "cengine/utils/log.h"

static UIElement *ui_element_new (void) {

    UIElement *ui_element = (UIElement *) malloc (sizeof (UIElement));
    if (ui_element) {
        ui_element->id = -1;
        ui_element->active = false;
        ui_element->layer_id = -1;
        ui_element->element = NULL;
    }

    return ui_element;

}

// ui element constructor
UIElement *ui_element_create (UI *ui, UIElementType type) {

    UIElement *new_element = NULL;

    // first check if we have a reusable ui element
    i32 spot = ui_element_get_free_spot ();

    // if (spot >= 0) {
    //     // FIXME: are we correctly removing all past elements from the layers??
    //     if (ui_elements[spot]) {
    //         new_element = ui_elements[spot];
    //         layer_remove_element (layer_get_by_pos (ui_elements_layers, new_element->layer_id), new_element);
    //         ui_element_delete_element (new_element);
    //     }

    //     else new_element = (UIElement *) malloc (sizeof (UIElement));
        
    //     new_element->id = spot;
    //     new_element->active = true;
    //     new_element->type = type;
    // }

    // else {
        if (ui->new_ui_element_id >= ui->max_ui_elements) ui_elements_realloc (ui);

        new_element = (UIElement *) malloc (sizeof (UIElement));
        if (new_element) {
            new_element->id = ui->new_ui_element_id;
            new_element->type = type;
            new_element->active = true;
            new_element->element = NULL;
            ui->ui_elements[new_element->id] = new_element;
            ui->new_ui_element_id++;
            ui->curr_max_ui_elements++;
        }
    // }

    // by default, add the ui element to the middle layer
    if (new_element) {
        Layer *layer = layer_get_by_name (ui_elements_layers, "middle");
        layer_add_element (layer, new_element);
        new_element->id = layer->pos;
    }

    return new_element;

}

static void ui_element_delete_element (UIElement *ui_element) {

    if (ui_element) {
        if (ui_element->element) {
            if (ui_element->element) {
                switch (ui_element->type) {
                    case UI_TEXTBOX: ui_textbox_delete (ui_element->element); break;
                    case UI_IMAGE: ui_image_delete (ui_element->element); break;
                    case UI_PANEL: ui_panel_delete (ui_element->element); break;
                    case UI_BUTTON: ui_button_delete (ui_element->element); break;
                    case UI_INPUT: ui_input_field_delete (ui_element->element); break;
                    case UI_CHECK: ui_check_delete (ui_element->element); break;
                    case UI_NOTI_CENTER: ui_noti_center_delete (ui_element->element); break;
                    case UI_DROPDOWN: ui_dropdown_delete (ui_element->element); break;

                    default: break;
                }

                ui_element->element = NULL;
            }
        }
    }

}

// deactivates the ui element and destroys its component (this is what the user should call)
void ui_element_destroy (UIElement *ui_element) {

    if (ui_element) {
        ui_element->id = -1;
        ui_element->active = false;
    }

}

// completely deletes the UI element (only called by dengine functions)
void ui_element_delete (UIElement *ui_element) {

    if (ui_element) {
        ui_element_destroy (ui_element);
        ui_element_delete_element (ui_element);
        free (ui_element);
    }

}

void ui_element_delete_dummy (void *ui_element_ptr) {}

int ui_element_comparator (const void *one, const void *two) {

    if (one && two) {
        UIElement *element_one = (UIElement *) one;
        UIElement *element_two = (UIElement *) two;

        if (element_one->id < element_two->id) return -1;
        else if (element_one->id == element_two->id) return 0;
        else return 1;
    }

    if (one) return -1;
    else if (two) return 1;
    return 0;

}

// sets the render layer of the ui element
// removes it from the one it is now and adds it to the new one
// returns 0 on success, 1 on error
int ui_element_set_layer (UIElement *ui_element, const char *layer_name) {

    int retval = 1;

    if (ui_element && layer_name) {
        Layer *layer = layer_get_by_name (ui_elements_layers, layer_name);
        if (layer) {
            Layer *curr_layer = layer_get_by_pos (ui_elements_layers, ui_element->layer_id);
            layer_remove_element (curr_layer, ui_element);

            retval = layer_add_element (layer, ui_element);
        }
    }

    return retval;

}

void ui_element_toggle_active (UIElement *ui_element) {

    if (ui_element) ui_element->active = !ui_element->active;

}

static UI *ui_new (void) {

    UI *ui = (UI *) malloc (sizeof (UI));
    if (ui) {
        ui->ui_elements = NULL;
        ui->max_ui_elements = 0;
        ui->curr_max_ui_elements = 0;
        ui->new_ui_element_id = 0;

        ui->ui_elements_layers = NULL;
    }

    return ui;

}

void ui_delete (void *ui_ptr) {

    if (ui_ptr) {
        UI *ui = (UI *) ui_ptr;

        if (ui->ui_elements) {
            for (u32 i = 0; i < ui->curr_max_ui_elements; i++)
                ui_element_delete (ui->ui_elements[i]);

            free (ui->ui_elements);
        }

        dlist_delete (ui->ui_elements_layers);

        free (ui_ptr);
    }

}

// init our ui elements structures
// returns 0 on success, 1 on error
u8 ui_create (UI *ui) {

    u8 retval = 1;

    if (ui) {
        ui->ui_elements = (UIElement **) calloc (DEFAULT_MAX_UI_ELEMENTS, sizeof (UIElement *));
        if (ui->ui_elements) {
            for (u32 i = 0; i < DEFAULT_MAX_UI_ELEMENTS; i++) ui->ui_elements[i] = NULL;

            ui->max_ui_elements = DEFAULT_MAX_UI_ELEMENTS;
            ui->curr_max_ui_elements = 0;
            ui->new_ui_element_id = 0;

            retval = 0;
        }
    }

    return retval;

}

static u8 ui_elements_realloc (UI *ui) {

    u8 retval = 1;

    if (ui) {
        u32 new_max_ui_elements = ui->curr_max_ui_elements * 2;

        ui->ui_elements = realloc (ui->ui_elements, new_max_ui_elements * sizeof (UIElement *));
        if (ui->ui_elements) {
            ui->max_ui_elements = new_max_ui_elements;
            retval = 0;;
        }
    }

    return retval;

}

static i32 ui_elements_get_free_spot (UI *ui) {

    if (ui) {
        for (u32 i = 0; i < ui->curr_max_ui_elements; i++) 
            if (ui->ui_elements[i]->id == -1)
                return (i32) i;
    }

    return -1;

}

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

#pragma region render

// render the ui elements to the screen
void ui_render (Renderer *renderer) {

    Layer *layer = NULL;
    UIElement *ui_element = NULL;
    for (ListElement *le = dlist_start (ui_elements_layers); le; le = le->next) {
        layer = (Layer *) le->data;

        for (ListElement *le_sub = dlist_start (layer->elements); le_sub; le_sub = le_sub->next) {
            ui_element = (UIElement *) le_sub->data;

            if (ui_element->active) {
                switch (ui_element->type) {
                    case UI_TEXTBOX: ui_textbox_draw ((TextBox *) ui_element->element, renderer); break;
                    case UI_IMAGE: ui_image_draw ((Image *) ui_element->element, renderer); break;
                    case UI_PANEL: ui_panel_draw ((Panel *) ui_element->element, renderer); break;
                    case UI_BUTTON: ui_button_draw ((Button *) ui_element->element, renderer); break;
                    case UI_INPUT: ui_input_field_draw ((InputField *) ui_element->element, renderer); break;
                    case UI_CHECK: ui_check_draw ((Check *) ui_element->element, renderer); break;
                    case UI_NOTI_CENTER: ui_noti_center_draw ((NotiCenter *) ui_element->element, renderer); break;
                    case UI_DROPDOWN: ui_dropdown_render ((Dropdown *) ui_element->element, renderer); break;

                    default: break;
                }
            }
        }   
    }

    // render the cursor on top of everything
    ui_cursor_draw (main_cursor, renderer);

}

#pragma endregion

#pragma region public

// init common ui elements
u8 ui_init (void) {

    int errors = 0;
    int retval = 0;

    // init and load fonts
    retval = ui_fonts_init ();
    errors |= retval;

    return errors;

}

// destroy common ui elements
u8 ui_end (void) {

    ui_cursor_delete (main_cursor);     // cursor

    ui_font_end ();     // fonts

    str_delete ((String *) ui_default_assets_path);

    #ifdef CENGINE_DEBUG
    cengine_log_msg (stdout, LOG_SUCCESS, LOG_NO_TYPE, "Done cleaning cengine ui.");
    #endif

    return 0;

}

#pragma endregion