#ifndef _CENGINE_UI_TEXTBOX_H_
#define _CENGINE_UI_TEXTBOX_H_

#include <stdbool.h>

#include "cengine/types/types.h"
#include "cengine/types/string.h"

#include "cengine/ui/ui.h"
#include "cengine/ui/font.h"
#include "cengine/ui/components/text.h"

typedef struct TextBox {

    Text *text;

    // background
    UIRect bgrect;
    RGBA_Color bgcolor;

} TextBox;

extern void ui_textbox_delete (void *textbox_ptr);

// sets the textbox font
extern void ui_textbox_set_font (TextBox *textbox, Font *font);

// sets the textbox's text
extern void ui_textbox_set_text (TextBox *textbox, const char *text, 
    Font *font, u32 size, RGBA_Color color);

// sets the textbox's text color
extern void ui_textbox_set_text_color (TextBox *textbox, RGBA_Color color);

// sets the textbox's background color
extern void ui_textbox_set_bg_color (TextBox *textbox, RGBA_Color color);

// creates a new textbox
extern TextBox *ui_textbox_create (u32 x, u32 y, u32 w, u32 h);

// draws the textbox
extern void ui_textbox_draw (TextBox *textbox);

#endif