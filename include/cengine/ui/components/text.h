#ifndef _CENGINE_UI_COMPONENT_TEXT_
#define _CENGINE_UI_COMPONENT_TEXT_

#include <SDL2/SDL.h>

#include "cengine/types/types.h"
#include "cengine/types/string.h"

#include "cengine/ui/ui.h"
#include "cengine/ui/font.h"

typedef struct Text {

    Font *font;
    FontSource *font_source;

    RGBA_Color text_color;
    u32 size;
    String *text;
    SDL_Rect rect;

    SDL_Texture *texture;       // rendered text

} Text;

extern Text *ui_text_component_new (void);
extern void ui_text_component_delete (void *text_ptr);

// inits the text component with the correct values
extern void ui_text_component_init (Text *text, Font *font, unsigned int size, RGBA_Color color, 
    const char *text_str);

    // sets a new text for the component
extern void ui_text_component_set_text (Text *text, const char *text_str);

// creates / updates the text texture
extern void ui_text_component_draw (Text *text);

// renders the text component to the screen
extern void ui_text_component_render (Text *text);

#endif