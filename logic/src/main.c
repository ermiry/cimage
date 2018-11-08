#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "cimage.h"

#include "log.h"
#include "utils/myUtils.h"

#pragma region OTHER

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

#pragma endregion

#pragma region JPEG

// process a COM marker
void jpeg_process_COM (const uchar *data, int length) {}

// process a SOFn marker -> used for image dimensions
void jpeg_process_SOFn (const uchar *data, int marker) {

}

void jpeg_checkAllocatedSections (void) {

}

// parse the marker stream until SOS or EOI is seen
int jpeg_readSections (FILE *file, ReadMode readmode) {

}

int jpeg_readFile (const char *filename, ReadMode readmode) {

    FILE *file = fopen (filename, "rb");
    if (!file) {
        logMsg (stderr, ERROR, IMAGE, createString ("Failed to open: %s", filename));
        return 1;
    }

    // scan the jpeg headers
    if (!jpeg_readSections (file, readmode)) {
        logMsg (stdout, DEBUG, IMAGE, "Done getting jpeg sections.");
        fclose (file);
        return 0;
    }

    else {
        logMsg (stdout, WARNING, IMAGE, createString ("Not jpeg: %s", filename));
        fclose (file);
        return 1;
    } 

}

#pragma endregion

#pragma region IMAGES

// do selected operations to one file a time
void cimage_processFile (const char *filename) {

    bool modified = false;

    ReadMode readmode = READ_METADATA;

    // TODO: reset jpg file

    // start with an empty image data
    ImageData *imgData = (ImageData *) malloc (sizeof (ImageData)); 
    imgData->flashUsed = -1;
    imgData->meteringMode = -1;
    imgData->whitebalance = -1;
    
    // TODO: store file date/time using fstat
    // thisis the creation day of the file in the current machine

    strncpy (imgData->filename, filename, 256);

    // no command option selected, so just read te img info
    if (jpeg_readFile (filename, readmode)) {
        // TODO: we got the exif header info
        // we can now display it
    }

    else {
        // TODO: discard data and return
    }

}

#pragma endregion

#pragma region THREAD

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

#pragma endregion