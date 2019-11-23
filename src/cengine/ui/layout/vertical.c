#include <stdlib.h>
#include <string.h>

#include "cengine/types/types.h"

#include "cengine/collections/dlist.h"

#include "cengine/ui/components/transform.h"
#include "cengine/ui/layout/vertical.h"

static VerticalLayout *ui_layout_vertical_new (void) {

    VerticalLayout *vertical = (VerticalLayout *) malloc (sizeof (VerticalLayout));
    if (vertical) {
        memset (vertical, 0, sizeof (VerticalLayout));
        vertical->transform = NULL;
        vertical->ui_elements = NULL;
    }

    return vertical;

}

void ui_layout_vertical_delete (void *vertical_ptr) {

    if (vertical_ptr) {
        VerticalLayout *vertical = (VerticalLayout *) vertical_ptr;

        ui_transform_component_delete (vertical->transform);
        dlist_delete (vertical->ui_elements);

        free (vertical);
    }

}