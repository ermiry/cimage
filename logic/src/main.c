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

// FIXME:
void jpeg_readEntireImage () {

    /* int cp, ep, size;
    // Determine how much file is left.
    cp = ftell(infile);
    fseek(infile, 0, SEEK_END);
    ep = ftell(infile);
    fseek(infile, cp, SEEK_SET);

    size = ep-cp;
    Data = (uchar *)malloc(size);
    if (Data == NULL){
        ErrFatal("could not allocate data for entire image");
    }

    got = fread(Data, 1, size, infile);
    if (got != size){
        ErrFatal("could not read the rest of the image");
    }

    CheckSectionsAllocated();
    Sections[SectionsRead].Data = Data;
    Sections[SectionsRead].Size = size;
    Sections[SectionsRead].Type = PSEUDO_IMAGE_MARKER;
    SectionsRead ++;
    HaveAll = 1; */

}

// FIXME:
void jpeg_processJFIF () {

    // Regular jpegs always have this tag, exif images have the exif
    // marker instead, althogh ACDsee will write images with both markers.
    // this program will re-create this marker on absence of exif marker.
    // hence no need to keep the copy from the file.

    /* if (memcmp(Data+2, "JFIF\0",5)){
        fprintf(stderr,"Header missing JFIF marker\n");
    }
    if (itemlen < 16){
        fprintf(stderr,"Jfif header too short\n");
        goto ignore;
    }

    ImageInfo.JfifHeader.Present = TRUE;
    ImageInfo.JfifHeader.ResolutionUnits = Data[9];
    ImageInfo.JfifHeader.XDensity = (Data[10]<<8) | Data[11];
    ImageInfo.JfifHeader.YDensity = (Data[12]<<8) | Data[13];
    if (ShowTags){
        printf("JFIF SOI marker: Units: %d ",ImageInfo.JfifHeader.ResolutionUnits);
        switch(ImageInfo.JfifHeader.ResolutionUnits){
            case 0: printf("(aspect ratio)"); break;
            case 1: printf("(dots per inch)"); break;
            case 2: printf("(dots per cm)"); break;
            default: printf("(unknown)"); break;
        }
        printf("  X-density=%d Y-density=%d\n",ImageInfo.JfifHeader.XDensity, ImageInfo.JfifHeader.YDensity);

        if (Data[14] || Data[15]){
            fprintf(stderr,"Ignoring jfif header thumbnail\n");
        }
    }

    ignore:

    free(Sections[--SectionsRead].Data); */

}

// TODO:
void jpeg_processSOFn () {}

int jpeg_handleSection (int marker, ReadMode readmode) {

    switch (marker) {
        // stop before hitting compressed data 
        case M_SOS: 
            // if reading entire image is requested, read the rest of the data
            if (readmode & READ_IMAGE) {
                jpeg_readEntireImage ();
                // FIXME: this return is from the function that called handle section!!
                // return TRUE;
            } 
            break;

         // used for jpeg quality guessing
        case M_DQT: /* FIXME: jpeg_processDQT(Data, itemlen); */ break;

        // also used for jpeg quality guessing
        case M_DHT: /* FIXME: process_DHT(Data, itemlen); */ break;

        // FIXME:
        case M_EOI:   // in case it's a tables-only JPEG stream
            // fprintf(stderr,"No image in jpeg!\n");
            // this is also from the func that called handle section ()
            // return FALSE;

        // FIXME:
        // comment section
        case M_COM: 
                /* if (HaveCom || ((ReadMode & READ_METADATA) == 0)){
                    // Discard this section.
                    free(Sections[--SectionsRead].Data);
                }else{
                    process_COM(Data, itemlen);
                    HaveCom = TRUE;
                }
                break; */

        // TODO: M_JFIF: jpeg_processJFIF (); break;

        // FIXME:
        case M_EXIF:
            // There can be different section using the same marker.
            /* if (ReadMode & READ_METADATA){
                if (memcmp(Data+2, "Exif", 4) == 0){
                    process_EXIF(Data, itemlen);
                    break;
                }else if (memcmp(Data+2, "http:", 5) == 0){
                    Sections[SectionsRead-1].Type = M_XMP; // Change tag for internal purposes.
                    if (ShowTags){
                        printf("Image cotains XMP section, %d bytes long\n", itemlen);
                        if (ShowTags){
                            ShowXmp(Sections[SectionsRead-1]);
                        }
                    }
                    break;
                }
            }
            // Oterwise, discard this section.
            free(Sections[--SectionsRead].Data);
            break; */

        // FIXME:
        case M_IPTC:
            /* if (ReadMode & READ_METADATA){
                if (ShowTags){
                    printf("Image cotains IPTC section, %d bytes long\n", itemlen);
                }
                // Note: We just store the IPTC section.  Its relatively straightforward
                // and we don't act on any part of it, so just display it at parse time.
            }else{
                free(Sections[--SectionsRead].Data);
            }
            break; */

        case M_SOF0: 
        case M_SOF1: 
        case M_SOF2: 
        case M_SOF3: 
        case M_SOF5: 
        case M_SOF6: 
        case M_SOF7: 
        case M_SOF9: 
        case M_SOF10:
        case M_SOF11:
        case M_SOF13:
        case M_SOF14:
        case M_SOF15:
            // jpeg_processSOFn (Data, marker);
            break;

        // skip any other section
        default: break;
    }

}

// parse the marker stream until SOS or EOI is seen
int jpeg_readSections (FILE *file, ReadMode readmode, ImageData *imgData) {

    //  int HaveCom = FALSE;

    int a = fgetc (file);
    if (a != 0xff || fgetc (file) != M_SOI) return 1;

    imgData->jfifHeader.xDensity = imgData->jfifHeader.yDensity = 300;
    imgData->jfifHeader.resolutionUnits = 1;

    // read jpeg sections based on marker
    int itemlen;
    int prev;
    int marker = 0;
    int ll,lh, got;
    uchar * Data;

    for (;;) {
        // TODO:
        jpeg_checkAllocatedSections ();

        // TODO: create a separate function for all the following...

        prev = 0;
        for (a = 0; ; a++) {
            marker = fgetc (file);
            if (marker != 0xff && prev == 0xff) break;
            if (marker == EOF) {
                // FIXME: 
                // fatal error -> just return? -> also discard the data
                logMsg (stderr,ERROR, IMAGE, "Unexpected end of file!");
            }
            prev = marker;
        }

        if (a > 10) {
            // FIXME: 
            // fatal error -> just return? -> also discard the data
            logMsg (stderr, ERROR, IMAGE, 
                createString ("Extraneous %d padding bytes before section %02X", a - 1, marker));
        }

        // FIXME: allocate sections -> do we need this to be global?
        // Sections[SectionsRead].Type = marker;

        // read the length of the section
        lh = fgetc (file);
        ll = fgetc (file);
        if (lh == EOF || ll == EOF) {
            // FIXME: 
            // fatal error -> just return? -> also discard the data
            logMsg (stderr, ERROR, IMAGE, "Unexpected end of file!");
        }

        itemlen = (lh << 8) | ll;

         if (itemlen < 2) {
            // FIXME: 
            // fatal error -> just return? -> also discard the data
            logMsg (stderr, ERROR, IMAGE, "Invalid marker!");
        }

        // FIXME: where do we put sections?
        // Sections[SectionsRead].Size = itemlen;

        Data = (uchar *) malloc (itemlen);
        // TODO: do we need this check?
        if (!Data) {
            // FIXME: 
            // fatal error -> just return? -> also discard the data
            // ErrFatal("Could not allocate memory");
        }

        // FIXME: where do we put sections?
        // Sections[SectionsRead].Data = Data;

        // Store first two pre-read bytes.
        Data[0] = (uchar) lh;
        Data[1] = (uchar) ll;

        // Read the whole section.
        got = fread (Data + 2, 1, itemlen - 2, file); 
        if (got != itemlen - 2) {
            // FIXME: 
            // fatal error -> just return? -> also discard the data
            logMsg (stderr, ERROR, IMAGE, "Premature end of file!");
        }

        // FIXME:
        // SectionsRead += 1;

        // now we handle the section
        // FIXME: jpeg_handleSection ();

    }

    return 0;   // sucess

}

int jpeg_readFile (const char *filename, ReadMode readmode, ImageData *imgData) {

    FILE *file = fopen (filename, "rb");
    if (!file) {
        logMsg (stderr, ERROR, IMAGE, createString ("Failed to open: %s", filename));
        return 1;
    }

    // scan the jpeg headers
    if (!jpeg_readSections (file, readmode, imgData)) {
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
    if (jpeg_readFile (filename, readmode, imgData)) {
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