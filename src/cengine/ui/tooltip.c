#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <SDL2/SDL_rect.h>

#include "cengine/collections/dlist.h"

#include "cengine/renderer.h"
#include "cengine/textures.h"

#include "cengine/ui/ui.h"
#include "cengine/ui/components/transform.h"
#include "cengine/ui/tooltip.h"

#include "cengine/ui/layout/layout.h"
#include "cengine/ui/layout/vertical.h"

static Tooltip *ui_tooltip_new (void) {

	Tooltip *tooltip = (Tooltip *) malloc (sizeof (Tooltip));
	if (tooltip) {
		tooltip->renderer = NULL;

		tooltip->ui_element = NULL;

		tooltip->colour = false;
		memset (&tooltip->bg_colour, 0, sizeof (RGBA_Color));
		tooltip->bg_texture = NULL;
		memset (&tooltip->bg_texture_rect, 0, sizeof (SDL_Rect));

		tooltip->vertical = NULL;

		tooltip->children = NULL;
	}

	return tooltip;

}

void ui_tooltip_delete (void *tooltip_ptr) {

	if (tooltip_ptr) {
		Tooltip *tooltip = (Tooltip *) tooltip_ptr;

		tooltip->ui_element = NULL;

		texture_destroy (tooltip->renderer, tooltip->bg_texture);

		ui_layout_vertical_delete (tooltip->vertical);

		dlist_delete (tooltip->children);

        free (tooltip);
	}

}

// sets the background colour of the tooltip
void ui_tooltip_set_bg_colour (Tooltip *tooltip, Renderer *renderer, RGBA_Color colour) {

    if (tooltip) {
        tooltip->colour = true;
        tooltip->bg_colour = colour;
        if (colour.a < 255) {
            render_complex_transparent_rect (renderer, &tooltip->bg_texture, &tooltip->ui_element->transform->rect, colour);
            tooltip->bg_texture_rect.w = tooltip->ui_element->transform->rect.w;
            tooltip->bg_texture_rect.h = tooltip->ui_element->transform->rect.h;
        }
    } 

}

// removes the background from the tooltip
void ui_tooltip_remove_background (Tooltip *tooltip) {

    if (tooltip) {
        if (tooltip->bg_texture) {
            texture_destroy (tooltip->renderer, tooltip->bg_texture);
            tooltip->bg_texture = NULL;
        }

        memset (&tooltip->bg_colour, 0, sizeof (RGBA_Color));
        tooltip->colour = false;
    }

}

Tooltip *ui_tooltip_create (Renderer *renderer) {

	Tooltip *tooltip = NULL;

	UIElement *ui_element = ui_element_create (renderer->ui, UI_TOOLTIP);
	if (ui_element) {
		tooltip = ui_tooltip_new ();
		if (tooltip) {
			tooltip->renderer = NULL;

			tooltip->ui_element	= ui_element;

			ui_element->element = tooltip;

			tooltip->vertical = ui_layout_vertical_create (
				// tooltip->ui_element->transform->rect.x, tooltip->ui_element->transform->rect.y, 
				// tooltip->ui_element->transform->rect.w, tooltip->ui_element->transform->rect.h,
				0, 0,
				0, 0,
				renderer
			);

			tooltip->children = dlist_init (ui_element_delete, ui_element_comparator);
		}
	}

	return tooltip;

}