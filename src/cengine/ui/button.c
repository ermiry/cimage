#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "cengine/types/types.h"
#include "cengine/types/string.h"
#include "cengine/types/vector2d.h"

#include "cengine/renderer.h"
#include "cengine/textures.h"
#include "cengine/sprites.h"
#include "cengine/input.h"

#include "cengine/ui/ui.h"
#include "cengine/ui/font.h"
#include "cengine/ui/button.h"
#include "cengine/ui/components/text.h"

static Button *ui_button_new (void) {

    Button *button = (Button *) malloc (sizeof (Button));
    if (button) {
        memset (button, 0, sizeof (Button));

        button->text = NULL;

        button->sprites = (Sprite **) calloc (BUTTON_STATE_TOTAL, sizeof (Sprite *));
        for (unsigned int  i = 0; i < BUTTON_STATE_TOTAL; i++)
            button->sprites[i] = NULL;

        button->ref_sprites = (bool *) calloc (BUTTON_STATE_TOTAL, sizeof (bool));
        for (unsigned int  i = 0; i < BUTTON_STATE_TOTAL; i++)
            button->ref_sprites[i] = false;

        button->action = NULL;
        button->args = NULL;
    }

    return button;

}

void ui_button_delete (void *button_ptr) {

    if (button_ptr) {
        Button *button = (Button *) button_ptr;

        ui_text_component_delete (button->text);

        if (button->sprites && button->ref_sprites) {
            for (unsigned int  i = 0; i < BUTTON_STATE_TOTAL; i++) {
                if (!button->ref_sprites[i]) sprite_destroy (button->sprites[i]);
                else button->sprites[i] = NULL;
            }

            free (button->sprites);
            free (button->ref_sprites);
        }

        free (button);
    }

}

// sets the button text
void ui_button_set_text (Button *button, const char *text, 
    Font *font, u32 size, RGBA_Color text_color) {

    if (button) {
        if (button->text) ui_text_component_delete (button->text);

        button->text = ui_text_component_new ();
        if (button->text) {
            ui_text_component_init (button->text,
                font, size, text_color, text);

            // set the text position inside the button
            button->text->rect.x = button->bgrect.x;
            button->text->rect.y = button->bgrect.y;

            ui_text_component_draw (button->text);
        }
    }

}

// sets the button's text color
void ui_button_set_text_color (Button *button, RGBA_Color color) {

    if (button) {
        button->text->text_color = color;
        ui_text_component_draw (button->text);
    }

}

// sets the background color of the button
void ui_button_set_bg_color (Button *button, RGBA_Color color) {

    if (button) button->bgcolor = color;

}

// sets an sprite for each button state
// the sprite is loaded and deleted when the button gets deleted
void ui_button_set_sprite (Button *button, ButtonState state, const char *filename) {

    if (button && filename) {
        Sprite *sprite = sprite_load (filename, main_renderer);

        if (sprite) {
            switch (state) {
                case BUTTON_STATE_MOUSE_OUT: 
                    button->sprites[BUTTON_STATE_MOUSE_OUT] = sprite;
                    button->ref_sprites[BUTTON_STATE_MOUSE_OUT] = false;
                    break;
                case BUTTON_STATE_MOUSE_OVER_MOTION: 
                    button->sprites[BUTTON_STATE_MOUSE_OVER_MOTION] = sprite;
                    button->ref_sprites[BUTTON_STATE_MOUSE_OVER_MOTION] = false;
                    break;
                case BUTTON_STATE_MOUSE_DOWN: 
                    button->sprites[BUTTON_STATE_MOUSE_DOWN] = sprite;
                    button->ref_sprites[BUTTON_STATE_MOUSE_DOWN] = false;
                    break;
                case BUTTON_STATE_MOUSE_UP: 
                    button->sprites[BUTTON_STATE_MOUSE_UP] = sprite;
                    button->ref_sprites[BUTTON_STATE_MOUSE_UP] = false;
                    break;

                default: break;
            }
        }
    }

}

// uses a refrence to the sprite and does not load or destroy it 
void ui_button_ref_sprite (Button *button, ButtonState state, Sprite *sprite) {

    if (button && sprite) {
        switch (state) {
            case BUTTON_STATE_MOUSE_OUT: 
                button->sprites[BUTTON_STATE_MOUSE_OUT] = sprite;
                button->ref_sprites[BUTTON_STATE_MOUSE_OUT] = true;
                break;
            case BUTTON_STATE_MOUSE_OVER_MOTION: 
                button->sprites[BUTTON_STATE_MOUSE_OVER_MOTION] = sprite;
                button->ref_sprites[BUTTON_STATE_MOUSE_OVER_MOTION] = true;
                break;
            case BUTTON_STATE_MOUSE_DOWN: 
                button->sprites[BUTTON_STATE_MOUSE_DOWN] = sprite;
                button->ref_sprites[BUTTON_STATE_MOUSE_DOWN] = true;
                break;
            case BUTTON_STATE_MOUSE_UP: 
                button->sprites[BUTTON_STATE_MOUSE_UP] = sprite;
                button->ref_sprites[BUTTON_STATE_MOUSE_UP] = true;
                break;

            default: break;
        }
    }

}

// sets an action to be triggered when the button is clicked
void ui_button_set_action (Button *button, Action action, void *args) {

    if (button) {
        button->action = action;
        button->args = args;
    } 

}

// creates a new button
Button *ui_button_create (u32 x, u32 y, u32 w, u32 h) {

    Button *button = NULL;

    UIElement *ui_element = ui_element_new (UI_BUTTON);
    if (ui_element) {
        button = ui_button_new ();
        if (button) {
            button->bgrect.x = x;
            button->bgrect.y = y;
            button->bgrect.w = w;
            button->bgrect.h = h;

            ui_element->element = button;
        }

        else ui_element_delete (ui_element);
    }

    return button;

}

// draws a button
void ui_button_draw (Button *button) {

    if (button) {
        Sprite *selected_sprite = NULL;

        // check if the mouse is in the button
        if (mousePos.x >= button->bgrect.x && mousePos.x <= (button->bgrect.x + button->bgrect.w) && 
            mousePos.y >= button->bgrect.y && mousePos.y <= (button->bgrect.y + button->bgrect.h)) {
            // check if the user pressed the left button over the mouse
            if (input_get_mouse_button_state (MOUSE_LEFT)) {
                button->pressed = true;
                selected_sprite = button->sprites[BUTTON_STATE_MOUSE_DOWN];
            }
               
            else if (!input_get_mouse_button_state (MOUSE_LEFT)) {
                if (button->pressed) {
                    button->pressed = false;
                    selected_sprite = button->sprites[BUTTON_STATE_MOUSE_UP];
                    if (button->action) button->action (button->args);
                    // printf ("Pressed!\n");
                }
            }

            // if not, the user is hovering the mouse over the button
            else selected_sprite = button->sprites[BUTTON_STATE_MOUSE_OVER_MOTION];
        }
    
        else button->pressed = false;

        // the mouse is NOT hovering over the button OR we dont have any other sprite available
        if (!selected_sprite)
            selected_sprite = button->sprites[BUTTON_STATE_MOUSE_OUT];

        if (selected_sprite) {
            selected_sprite->dest_rect.x = button->bgrect.x;
            selected_sprite->dest_rect.y = button->bgrect.y;

            SDL_RenderCopyEx (main_renderer->renderer, selected_sprite->texture, 
                &selected_sprite->src_rect, 
                &selected_sprite->dest_rect, 
                0, 0, NO_FLIP);
        } 

        render_basic_outline_rect (&button->bgrect, button->bgcolor);

        // draw button text
        if (button->text) {
            SDL_RenderCopy (main_renderer->renderer, button->text->texture, 
                NULL, &button->text->rect);
        }
    }

}