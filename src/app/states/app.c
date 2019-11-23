#include <stdlib.h>

#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <dirent.h>

#include "cengine/types/types.h"
#include "cengine/types/string.h"

#include "cengine/collections/dlist.h"

#include "cengine/threads/thread.h"
#include "cengine/manager/manager.h"
#include "cengine/game/go.h"

#include "cengine/utils/utils.h"
#include "cengine/utils/log.h"

#include "app/ui/app.h"

#pragma region images

static DoubleList *images = NULL;

// FIXME: also read jpeg images
static bool is_image_file (const char *filename) {

    int result;
    bool retval = false;

    if (filename) {
        char temp[3];
        unsigned int len = strlen (filename);

        temp[2] = filename[len - 1];
        temp[1] = filename[len - 2];
        temp[0] = filename[len - 3];

        if (!strcmp (temp, "jpg")) retval = true;
        else if (!strcmp (temp, "png")) retval = true;
    }

    return retval;

}

// get a list of valid images names from the directory
DoubleList *images_read_from_dir (const char *images_dir) {

    DoubleList *images = NULL;

    if (images_dir) {
        images = dlist_init (str_delete, str_comparator);
        
        struct dirent *ep = NULL;
        DIR *dp = opendir (images_dir);
        if (dp) {
            #ifdef CIMAGE_DEBUG
            cengine_log_msg (stdout, LOG_SUCCESS, LOG_NO_TYPE, "Starting to read images...");
            #endif

            while ((ep = readdir (dp)) != NULL) {
                if (strcmp (ep->d_name, ".") && strcmp (ep->d_name, "..")) {
                    if (is_image_file (ep->d_name)) {
                        String *filename = str_create ("%s/%s", images_dir, ep->d_name);
                        dlist_insert_after (images, dlist_end (images), filename);
                    }
                }
            }

            (void) closedir (dp);
        }

        else {
            char *status = c_string_create ("Failed to open dir: %s", images_dir);
            if (status) {
                cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, status);
                free (status);
            }
        }
    }

    return images;

}

#pragma endregion

State *app_state = NULL;

static void app_update (void) {

    game_object_update_all ();

}

static void *app_load_images (void *ptr) {

    // get images from directory
    images = images_read_from_dir ("./images");
    if (images) {
        for (ListElement *le = dlist_start (images); le; le = le->next) {
            printf ("%s\n", ((String *) le->data)->str);
            app_ui_image_display (((String *) le->data)->str);
        }
    }

    return NULL;

}

static void app_on_enter (void) { 

    app_state->update = app_update;

    app_ui_init ();

    // thread_create_detachable (app_load_images, NULL);
    // images = images_read_from_dir ("./images");
    // if (images) {
    //     for (ListElement *le = dlist_start (images); le; le = le->next) {
    //         printf ("%s\n", ((String *) le->data)->str);
    //         app_ui_image_display (((String *) le->data)->str);
    //     }
    // }

}

static void app_on_exit (void) { 

    app_ui_end ();

    dlist_delete (images);

}

State *app_state_new (void) {

    State *new_app_state = (State *) malloc (sizeof (State));
    if (new_app_state) {
        // new_game_state->state = IN_GAME;

        new_app_state->update = NULL;

        new_app_state->on_enter = app_on_enter;
        new_app_state->on_exit = app_on_exit;
    }

}