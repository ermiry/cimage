#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "cengine/types/types.h"
#include "cengine/types/string.h"

#include "cengine/renderer.h"
#include "cengine/ui/ui.h"
#include "cengine/ui/font.h"
#include "cengine/ui/textbox.h"
#include "cengine/ui/components/text.h"

static TextBox *ui_textbox_new (void) {

    TextBox *textbox = (TextBox *) malloc (sizeof (TextBox));
    if (textbox) {
        memset (textbox, 0, sizeof (TextBox));

        textbox->text = NULL;
    }

    return textbox;

}

void ui_textbox_delete (void *textbox_ptr) {

    if (textbox_ptr) {
        TextBox *textbox = (TextBox *) textbox_ptr;

        ui_text_component_delete (textbox->text);

        free (textbox);
    }

}

// sets the textbox font
void ui_textbox_set_font (TextBox *textbox, Font *font) {

    if (textbox) {
        textbox->text->font = font;
        ui_text_component_draw (textbox->text);
    }

}

// sets the textbox's text
void ui_textbox_set_text (TextBox *textbox, const char *text, 
    Font *font, u32 size, RGBA_Color color) {

    if (textbox) {
        if (textbox->text) ui_text_component_delete (textbox->text);

        textbox->text = ui_text_component_new ();
        if (textbox->text) {
            ui_text_component_init (textbox->text,
                font, size, color, text);

            // set the text position inside the textbox
            textbox->text->rect.x = textbox->bgrect.x;
            textbox->text->rect.y = textbox->bgrect.y;

            ui_text_component_draw (textbox->text);
        }
    }

}

// sets the textbox's text color
void ui_textbox_set_text_color (TextBox *textbox, RGBA_Color color) {

    if (textbox) {
        textbox->text->text_color = color;
        ui_text_component_draw (textbox->text);
    }

}

// sets the textbox's background color
void ui_textbox_set_bg_color (TextBox *textbox, RGBA_Color color) {

    if (textbox) textbox->bgcolor = color;

}

// creates a new textbox
TextBox *ui_textbox_create (u32 x, u32 y, u32 w, u32 h) {

    TextBox *textbox = NULL;

    UIElement *ui_element = ui_element_new (UI_TEXTBOX);
    if (ui_element) {
        textbox = ui_textbox_new ();

        if (textbox) {
            textbox->bgrect.x = x;
            textbox->bgrect.y = y;   
            textbox->bgrect.w = w;
            textbox->bgrect.h = h;           

            ui_element->element = textbox;
        }

        else ui_element_delete (ui_element);
    }

    return textbox;

}

// draws the textbox
void ui_textbox_draw (TextBox *textbox) {

    if (textbox) {
        SDL_RenderCopy (main_renderer->renderer, textbox->text->texture, 
            NULL, &textbox->text->rect);
    }

}