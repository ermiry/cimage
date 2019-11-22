#ifndef _CENGINE_UI_H_
#define _CENGINE_UI_H_

#include <stdbool.h>

#include <SDL2/SDL_rect.h>

#include "cengine/types/types.h"
#include "cengine/collections/dlist.h"

#include "cengine/renderer.h"

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
extern UIElement *ui_element_create (UI *ui, UIElementType type);

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
extern int ui_element_set_layer (UIElement *ui_element, const char *layer_name);

extern void ui_element_toggle_active (UIElement *ui_element);

typedef struct UI {

    UIElement **ui_elements;
    u32 max_ui_elements;
    u32 curr_max_ui_elements;
    u32 new_ui_element_id;

    DoubleList *ui_elements_layers;

} UI;

extern void ui_delete (void *ui_ptr);

// init our ui elements structures
// returns 0 on success, 1 on error
extern u8 ui_create (UI *ui);

/*** Public ui funcs ***/

// renders all the current active ui to the screen
extern void ui_render (Renderer *renderer);

// initializes cengine's ui capabilities
extern u8 ui_init (void);

// sets the location of cengine's default ui assets
extern void ui_default_assets_set_path (const char *pathname);

// loads cengine's default ui assets
extern u8 ui_default_assets_load (void);

// destroy common ui elements
extern u8 ui_end (void);

#endif