#ifndef _CENGINE_UI_INPUT_FIELD_H_
#define _CENGINE_UI_INPUT_FIELD_H_

#include <stdbool.h>

#include "cengine/types/types.h"
#include "cengine/types/string.h"

#include "cengine/ui/ui.h"
#include "cengine/ui/font.h"
#include "cengine/ui/components/text.h"

typedef struct InputField {

    // placeholder text
    Text *placeholder;
    bool empty_text;

    // text
    Text *text;
    bool is_password;
    String *password;

    // background
    UIRect bgrect;
    RGBA_Color bgcolor;

    // outline
    bool draw_selected;
    RGBA_Color selected_color;

    bool pressed;

} InputField;

extern void ui_input_field_delete (void *input_ptr);

// sets the input placeholder text
extern void ui_input_field_set_placeholder_text (InputField *input, const char *text,
    Font *font, u32 size, RGBA_Color text_color);

// sets the input field's text
void ui_input_field_set_text (InputField *input, const char *text,
    Font *font, u32 size, RGBA_Color text_color, bool is_password);

// sets the input field's text color
extern void ui_input_field_set_text_color (InputField *input, RGBA_Color color);

// returns the actual password value
extern String *ui_input_field_get_password (InputField *input);

// sets the input field's background color
extern void ui_input_field_set_bg_color (InputField *input, RGBA_Color color);

// creates a new input field
extern InputField *ui_input_field_create (u32 x, u32 y, u32 w, u32 h);

// draws the input field
extern void ui_input_field_draw (InputField *input);

// updates the input field with the correct values
extern void ui_input_field_update (InputField *input);

// sets the option to draw an outline rect when the input is selected
extern void ui_input_field_set_selected (InputField *input, RGBA_Color selected_color);

#endif