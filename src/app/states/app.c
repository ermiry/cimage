#include <stdlib.h>

#ifdef CIMAGE_DEBUG
#include <errno.h>
#endif

#include <unistd.h>

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
static DoubleList *images_folder_read (const char *images_dir) {

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

            #ifdef CIMAGE_DEBUG
            perror ("Error");
            #endif
        }
    }

    return images;

}

// TODO: check if we could load any image
static void *images_load (void *folder_name_ptr) {

    if (folder_name_ptr) {
        String *folder_name = (String *) folder_name_ptr;

        // get images from directory
        images = images_folder_read (folder_name->str);
        if (images) {
            // prepare ui for images
            app_ui_images_set_ui_elements ();

            for (ListElement *le = dlist_start (images); le; le = le->next) {
                printf ("%s\n", ((String *) le->data)->str);
                app_ui_image_display (((String *) le->data)->str);
            }
        }
    }

    return NULL;

}

// TODO: implement cross-platform code
void images_folder_select (void *args) {

    #ifdef OS_LINUX
    char *command = NULL;
    char *username = getlogin ();
    if (username) {
        // printf ("%s\n", username);
        command = c_string_create ("zenity  --file-selection --title=\"Choose a photos directory\" --filename=/home/%s/ --save --directory", 
            username);
    }

    else {
        // if for whatever reason we can get the username, just open in root folder
        command = c_string_create ("zenity  --file-selection --title=\"Choose a photos directory\" --filename=/ --save --directory");
    }

    if (command) {
        char folder_name[1024];
        FILE *pipe = popen (command, "r");
        if (pipe) {
            fgets (folder_name, 1024, pipe);
            fclose (pipe);
            // printf ("\n%s\n", folder_name);

            c_string_remove_char (folder_name, '\n');
            String *selected_folder = str_new (folder_name);

            thread_create_detachable (images_load, selected_folder);
        }

        free (command);
    }
    #endif

}

#pragma endregion

State *app_state = NULL;

static void app_update (void) {

    game_object_update_all ();

}

static void app_on_enter (void) { 

    app_state->update = app_update;

    app_ui_init ();

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