#include <stdlib.h>
#include <string.h>

#include "cengine/types/types.h"

#include "cengine/renderer.h"

#include "cengine/ui/ui.h"
#include "cengine/ui/panel.h"
#include "cengine/ui/components/transform.h"

static Panel *ui_panel_new (void) {

    Panel *panel = (Panel *) malloc (sizeof (Panel));
    if (panel) {
        memset (panel, 0, sizeof (Panel));
        panel->ui_element = NULL;
        panel->transform = NULL;
        panel->texture = NULL;
    }

    return panel;

}

void ui_panel_delete (void *panel_ptr) {

    if (panel_ptr) {
        Panel *panel = (Panel *) panel_ptr;
        panel->ui_element = NULL;
        ui_transform_component_delete (panel->transform);
        if (panel->texture) SDL_DestroyTexture (panel->texture);
        free (panel);
    }

}

// sets the background colour of the panel
void ui_panel_set_bg_colour (Panel *panel, RGBA_Color colour) {

    if (panel) {
        panel->bgcolor = colour;
        if (colour.a < 255) {
            panel->texture = render_complex_transparent_rect (&panel->transform->rect, colour);
            panel->texture_rect.w = panel->transform->rect.w;
            panel->texture_rect.h = panel->transform->rect.h;
        }
    } 

}

// creates a new panel
// x and y for position
Panel *panel_create (u32 x, u32 y, u32 w, u32 h, UIPosition pos) {

    Panel *panel = NULL;

    UIElement *ui_element = ui_element_new (UI_PANEL);
    if (ui_element) {
        panel = ui_panel_new ();
        if (panel) {
            panel->ui_element = ui_element;
            panel->transform = ui_transform_component_create (x, y, w, h);
            ui_transform_component_set_pos (panel->transform, NULL, pos);
            ui_element->element = panel;
        }
    }

    return panel;

}

// draws the panel to the screen
void ui_panel_draw (Panel *panel) {

    if (panel->texture) {
        SDL_RenderCopyEx (main_renderer->renderer, panel->texture, 
            &panel->texture_rect, &panel->transform->rect, 
            0, 0, SDL_FLIP_NONE);
    }

    else {
        render_basic_filled_rect (&panel->transform->rect, panel->bgcolor);
    }

}