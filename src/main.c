#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cimage.h"

/*** LOG ***/

char *getMsgType (LogMsgType type) {

    char temp[10];

    switch (type) {
        case ERROR: strcpy (temp, "[ERROR]"); break;
        case WARNING: strcpy (temp, "[WARNING]"); break;
        case SUCCESS: strcpy (temp, "[SUCCESS]"); break;
        case DEBUG: strcpy (temp, "[DEBUG]"); break;
        case TEST: strcpy (temp, "[TEST]"); break;

        default: break;
    }

    char *retval = (char *) calloc (strlen (temp) + 1, sizeof (temp));
    strcpy (retval, temp);

    return retval;

}

void logMsg (FILE *__restrict __stream, LogMsgType firstType, LogMsgType secondType,
    const char *msg) {

    char *first = getMsgType (firstType);
    char *second = NULL;

    char *message = NULL;

    if (secondType != 0) {
        second = getMsgType (secondType);
        message = createString ("%s%s: %s\n", first, second, msg);
    }

    else message = createString ("%s: %s\n", first, msg);

    // log messages with color
    switch (firstType) {
        case ERROR: fprintf (__stream, COLOR_RED "%s" COLOR_RESET, message); break;
        case WARNING: fprintf (__stream, COLOR_YELLOW "%s" COLOR_RESET, message); break;
        case SUCCESS: fprintf (__stream, COLOR_GREEN "%s" COLOR_RESET, message); break;

        default: fprintf (__stream, "%s", message); break;
    }

    if (!first) free (first);
    if (!second) free (second);

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
           "  -v    version     Display current version.\n");
}

// TODO: maybe if no arg is provided we can launch the cimage cmd line app?
// TODO: hanlde more than one argument each time
// TODO: handle when we need an extra argument
int main (int argc, char **argv) {

    if (argc <= 1) {
        logMsg (stderr, ERROR, NO_TYPE, "No option provided! Use -h for help.");
        exit (EXIT_FAILURE);
    }

    int argn;
    char *arg;
    char *progname = argv[0];

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
    
    // TODO: search for the file to process...
          
    return EXIT_SUCCESS;

}