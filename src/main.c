#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

#include "cimage.h"
#include "app/app.h"
// #include "ui/ui.h"

#include "cengine/input.h"
#include "cengine/renderer.h"
#include "cengine/animation.h"

#include "utils/log.h"
#include "utils/myUtils.h"

// bool running = true;
bool inGame = false;
bool wasInGame = false;

void quit (int code) {

    // running = false;
    // inGame = false;

}

void die (const char *error) {

    logMsg (stderr, ERROR, NO_TYPE, error);
    quit (1);

};

static int init (void) {

    int errors = 0;

    // register to some signals
    signal (SIGINT, quit);
    signal (SIGSEGV, quit);
    signal (SIGABRT, quit);

    // TODO: add settings
    // main_settings = settings_load ();

    // FIXME: init cengine

    return errors;

}

// TODO: load this from a file
void printUsage (void) {

    fprintf (stdout, "\ncimage v."CIMAGE_VERSION" by Erick Salas, Oct 2018\n\
            \ncimage is a program for reading exif data in jpg images. It can be also used "
            "to manipulate image files, like sorting by date.\n");

    fprintf (stdout, "\nUsage: cimage [options] files\n");

    fprintf (stdout, "Where:\n"
           " files       path/filenames with or without wildcards\n"

           "[options] are:\n"
           "  -h    help        This text.\n"
           "  -v    version     Display current version.\n");\
           
}

// TODO: maybe if no arg is provided we can launch the cimage cmd line app?
// TODO: hanlde more than one argument each time
// TODO: handle when we need an extra argument
int main (int argc, char **argv) {

    /* if (argc <= 1) {
        logMsg (stderr, ERROR, NO_TYPE, "No option provided! Use -h for help.");
        exit (EXIT_FAILURE);
    }

    int argn;
    char *arg;
    char *progname = argv[0];

    // we have an option + file(s)
    if (argc > 2) {
        for (argn = 1; argn < argc; argn++) {
            arg = argv[argn];

            if (arg[0] != '-') break;  // we expect only one image then...

            // general options
            if (!strcmp (arg, "-v")) 
                printf ("\ncimage version: "CIMAGE_VERSION"   Compiled: "__DATE__"\n");    

            else if (!strcmp (arg, "-h")) printUsage ();

            else {
                printf ("\n[ERROR]: Argument '%s' not understood.\n", arg);
                printf ("Use -h for a list of commands.\n");
                exit (EXIT_FAILURE);
            }
        }
    }

    // we just read a filename and we process the file
    else cimage_processFile (argv[1]); */

    // FIXME: check blackrock code for main function...
    // running = !init () ? true : false;

    // app_state = app_state_new ();
    // app_manager = app_manager_new (app_state);

    // run ();
          
    // return end ();

    return 0;

}