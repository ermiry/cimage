#include <stdlib.h>
#include <string.h>

#include "cengine/types/types.h"

#include "cengine/collections/dlist.h"

#include "cengine/ui/components/transform.h"
#include "cengine/ui/layout/horizontal.h"

static HorizontalLayout *ui_layout_horizontal_new (void) {

    HorizontalLayout *horizontal = (HorizontalLayout *) malloc (sizeof (HorizontalLayout));
    if (horizontal) {
        memset (horizontal, 0, sizeof (HorizontalLayout));
        horizontal->transform = NULL;
        horizontal->ui_elements = NULL;
    }

    return horizontal;

}

void ui_layout_horizontal_delete (void *horizontal_ptr) {

    if (horizontal_ptr) {
        HorizontalLayout *horizontal = (HorizontalLayout *) horizontal_ptr;

        ui_transform_component_delete (horizontal->transform);
        dlist_delete (horizontal->ui_elements);

        free (horizontal);
    }

}