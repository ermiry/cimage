#ifndef _CENGINE_INPUT_H_
#define _CENGINE_INPUT_H_

#include <SDL2/SDL.h>

#include "types/vector2D.h"

#define N_MOUSE_BUTTONS     3

typedef enum MouseButton {

    MOUSE_LEFT = 0,
    MOUSE_MIDDLE = 1,
    MOUSE_RIGHT = 2

} MouseButton;

extern Vector2D mousePos;

extern bool input_get_mouse_button_state (MouseButton button);

extern bool input_is_key_down (const SDL_Scancode key);

extern void input_init (void);
extern void input_handle (SDL_Event event);

#endif