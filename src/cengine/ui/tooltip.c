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