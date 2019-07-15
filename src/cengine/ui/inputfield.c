#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "cengine/types/types.h"
#include "cengine/types/string.h"
#include "cengine/types/vector2d.h"

#include "cengine/renderer.h"
#include "cengine/input.h"

#include "cengine/ui/ui.h"
#include "cengine/ui/font.h"
#include "cengine/ui/inputfield.h"
#include "cengine/ui/components/text.h"

extern void ui_input_field_update (InputField *input);

static InputField *ui_input_field_new (void) {

    InputField *input = (InputField *) malloc (sizeof (InputField));
    if (input) {
        memset (input, 0, sizeof (InputField));

        input->placeholder = NULL;
        input->empty_text = true;

        input->text = NULL;
        input->password = NULL;
        input->is_password = false;

        input->pressed = false;
    }

    return input;

}

void ui_input_field_delete (void *input_ptr) {

    if (input_ptr) {
        InputField *input = (InputField *) input_ptr;

        ui_text_component_delete (input->placeholder);
        ui_text_component_delete (input->text);
        str_delete (input->password);

        free (input);
    }

}

// sets the input placeholder text
void ui_input_field_set_placeholder_text (InputField *input, const char *text,
    Font *font, u32 size, RGBA_Color text_color) {

    if (input) {
        if (input->placeholder) ui_text_component_delete (input->placeholder);

        input->placeholder = ui_text_component_new ();
        if (input->placeholder) {
            ui_text_component_init (input->placeholder,
                font, size, text_color, text);

            // set the text position inside the input field
            input->placeholder->rect.x = input->bgrect.x;
            input->placeholder->rect.y = input->bgrect.y;

            ui_text_component_draw (input->placeholder);
        }
    }

}

// sets the input field's text
void ui_input_field_set_text (InputField *input, const char *text,
    Font *font, u32 size, RGBA_Color text_color, bool is_password) {

    if (input) {
        if (input->text) ui_text_component_delete (input->text);

        input->text = ui_text_component_new ();
        if (input->text) {
            ui_text_component_init (input->text,
                font, size, text_color, text);

            // set the text position inside the input field
            input->text->rect.x = input->bgrect.x;
            input->text->rect.y = input->bgrect.y;

            ui_text_component_draw (input->text);
        }

        if (is_password) {
            input->is_password = true;
            input->password = text ? str_new (text) : str_new ("");
            ui_input_field_update (input);
        } 
    }

}

// updates the placeholder text (redraws the text component)
void ui_input_field_update_text (InputField *input, const char *update_text) {

    if (input) {
        if (input->text) {
            ui_text_component_set_text (input->text, update_text);
            ui_text_component_draw (input->text);
        }
    }

}

// sets the input field's text color
void ui_input_field_set_text_color (InputField *input, RGBA_Color color) {

    if (input) {
        input->text->text_color = color;
        ui_text_component_draw (input->text);
    }

}

// returns the actual password value
String *ui_input_field_get_password (InputField *input) {

    if (input) return input->password;

}

// sets the input field's background color
void ui_input_field_set_bg_color (InputField *input, RGBA_Color color) {

    if (input) input->bgcolor = color;

}

// sets the option to draw an outline rect when the input is selected
void ui_input_field_set_selected (InputField *input, RGBA_Color selected_color) {

    if (input) {
        input->draw_selected = true;
        input->selected_color = selected_color;
    }

}

// creates a new input field
InputField *ui_input_field_create (u32 x, u32 y, u32 w, u32 h) {

    InputField *input = NULL;

    UIElement *ui_element = ui_element_new (UI_INPUT);
    if (ui_element) {
        input = ui_input_field_new ();

        if (input) {
            input->bgrect.x = x;
            input->bgrect.y = y;
            input->bgrect.w = w;
            input->bgrect.h = h;

            ui_element->element = input;
        }

        else ui_element_delete (ui_element);
    }

    return input;

}

// draws the input field
void ui_input_field_draw (InputField *input) {

    if (input) {
        // check if the mouse is in the input
        if (mousePos.x >= input->bgrect.x && mousePos.x <= (input->bgrect.x + input->bgrect.w) && 
            mousePos.y >= input->bgrect.y && mousePos.y <= (input->bgrect.y + input->bgrect.h)) {
            // check if the user pressed the left input over the mouse
            if (input_get_mouse_button_state (MOUSE_LEFT)) {
                input->pressed = true;
            }
               
            else if (!input_get_mouse_button_state (MOUSE_LEFT)) {
                if (input->pressed) {
                    input->pressed = false;
                    // make this text active
                    input_set_active_text (input);
                    // printf ("Pressed!\n");
                }
            }
        }
    
        else input->pressed = false;

        if (input->pressed) {
            if (input->draw_selected) 
                render_basic_outline_rect (&input->bgrect, input->selected_color);
        }

        else render_basic_outline_rect (&input->bgrect, RGBA_WHITE);
        
        // draw the correct text
        if (input->empty_text) {
            // draw the placeholder text
            ui_text_component_render (input->placeholder);
        }

        else {
            // draw the input text
            ui_text_component_render (input->text);
        }
    }

}

// updates the input field with the correct values
void ui_input_field_update (InputField *input) {

    if (input) {
        // check if the input text is empty
        if (input->is_password) {
            if (strcmp (input->password->str, "")) {
                input->empty_text = false;

                // update the input's text
                unsigned int pass_len = strlen (input->password->str);
                char *secret = (char *) calloc (pass_len, sizeof (char));
                memset (secret, '*', pass_len);
                str_delete (input->text->text);
                input->text->text = str_new (secret);
                free (secret);
            }
            
            else input->empty_text = true;
        }

        else {
            if (strcmp (input->text->text->str, ""))
                input->empty_text = false;
            else input->empty_text = true;
        }

        ui_text_component_draw (input->text);
    }

}