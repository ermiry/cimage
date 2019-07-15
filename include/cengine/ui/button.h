#ifndef _CENGINE_UI_BUTTON_H_
#define _CENGINE_UI_BUTTON_H_

#include <stdbool.h>

#include "cengine/types/types.h"
#include "cengine/types/string.h"

#include "cengine/sprites.h"

#include "cengine/ui/ui.h"
#include "cengine/ui/font.h"
#include "cengine/ui/components/text.h"

typedef enum ButtonState {

    BUTTON_STATE_MOUSE_OUT = 0,
    BUTTON_STATE_MOUSE_OVER_MOTION = 1,
    BUTTON_STATE_MOUSE_DOWN = 2,
    BUTTON_STATE_MOUSE_UP = 3,
    BUTTON_STATE_TOTAL = 4

} ButtonState;

typedef struct Button {

    Text *text;

    // background
    UIRect bgrect;
    RGBA_Color bgcolor;

    Sprite **sprites;
    bool *ref_sprites;

    // event listener
    bool pressed;
    Action action;
    void *args;

} Button;

extern void ui_button_delete (void *button_ptr);

// sets the button text
// sets the button text
extern void ui_button_set_text (Button *button, const char *text, 
    Font *font, u32 size, RGBA_Color text_color);

// sets the button's text color
extern void ui_button_set_text_color (Button *button, RGBA_Color color);

// sets the background color of the button
extern void ui_button_set_bg_color (Button *button, RGBA_Color color);

// sets an sprite for each button state
// the sprite is loaded and deleted when the button gets deleted
extern void ui_button_set_sprite (Button *button, ButtonState state, const char *filename);

// uses a refrence to the sprite and does not load or destroy it 
extern void ui_button_ref_sprite (Button *button, ButtonState state, Sprite *sprite);

// sets an action to be triggered when the button is clicked
extern void ui_button_set_action (Button *button, Action action, void *args);

// creates a new button
extern Button *ui_button_create (u32 x, u32 y, u32 w, u32 h);

// draws a button
extern void ui_button_draw (Button *button);

#endif