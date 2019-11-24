#ifndef _CENGINE_UI_H_
#define _CENGINE_UI_H_

#include <stdbool.h>

#include <SDL2/SDL_rect.h>

#include "cengine/types/types.h"
#include "cengine/collections/dlist.h"

#include "cengine/window.h"
#include "cengine/renderer.h"

struct _Window;
struct _Renderer;
struct _UI;

typedef SDL_Rect UIRect;

typedef enum UIElementType {

    UI_TEXTBOX,
    UI_IMAGE,
    UI_PANEL,
    UI_BUTTON,
    UI_INPUT,
    UI_CHECK,
    UI_NOTI_CENTER,
    UI_DROPDOWN

} UIElementType;

#define DEFAULT_MAX_UI_ELEMENTS         64

typedef struct UIElement {

    i32 id;
    bool active;
    UIElementType type;
    int layer_id;
    void *element;

} UIElement;

// ui element constructor
extern UIElement *ui_element_create (struct _UI *ui, UIElementType type);

// deactivates the ui element and destroys its component (this is what the user should call)
extern void ui_element_destroy (UIElement *ui_element);

// completely deletes the UI element (only called by dengine functions)
extern void ui_element_delete (UIElement *ui_element);

extern void ui_element_delete (UIElement *ui_element);

extern void ui_element_delete_dummy (void *ui_element_ptr);

extern int ui_element_comparator (const void *one, const void *two);

// sets the render layer of the ui element
// removes it from the one it is now and adds it to the new one
// returns 0 on success, 1 on error
extern int ui_element_set_layer (struct _UI *ui, UIElement *ui_element, const char *layer_name);

extern void ui_element_toggle_active (UIElement *ui_element);

struct _UI {

    UIElement **ui_elements;
    u32 max_ui_elements;
    u32 curr_max_ui_elements;
    u32 new_ui_element_id;

    DoubleList *ui_elements_layers;

};

typedef struct _UI UI;

extern void ui_delete (void *ui_ptr);

// init our ui elements structures
extern UI *ui_create (void);

/*** Public ui funcs ***/

// resize the ui elements to fit new window
extern void ui_resize (struct _Window *window);

// renders all the current active ui to the screen
extern void ui_render (struct _Renderer *renderer);

// initializes cengine's ui capabilities
extern u8 ui_init (void);

// destroy common ui elements
extern u8 ui_end (void);

#endif