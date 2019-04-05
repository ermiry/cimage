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

static int Get16m (const void *Short) {

    return (((uchar *) Short)[0] << 8) | ((uchar *) Short)[1];

}


#pragma endregion

#pragma region EXIF

void exif_process (unsigned char * ExifSection, unsigned int length) {

    /* unsigned int FirstOffset;

    FocalplaneXRes = 0;
    FocalplaneUnits = 0;
    ExifImageWidth = 0;
    NumOrientations = 0;

    // if (ShowTags){
    //     printf("Exif header %u bytes long\n",length);
    // }

    {   // Check the EXIF header component
        static uchar ExifHeader[] = "Exif\0\0";
        if (memcmp(ExifSection+2, ExifHeader,6)){
            ErrNonfatal("Incorrect Exif header",0,0);
            return;
        }
    }

    if (memcmp(ExifSection+8,"II",2) == 0){
        if (ShowTags) printf("Exif section in Intel order\n");
        MotorolaOrder = 0;
    }else{
        if (memcmp(ExifSection+8,"MM",2) == 0){
            if (ShowTags) printf("Exif section in Motorola order\n");
            MotorolaOrder = 1;
        }else{
            ErrNonfatal("Invalid Exif alignment marker.",0,0);
            return;
        }
    }

    // Check the next value for correctness.
    if (Get16u(ExifSection+10) != 0x2a){
        ErrNonfatal("Invalid Exif start (1)",0,0);
        return;
    }

    FirstOffset = Get32u(ExifSection+12);
    if (FirstOffset < 8 || FirstOffset > 16){
        if (FirstOffset < 16 || FirstOffset > length-16){
            ErrNonfatal("invalid offset for first Exif IFD value",0,0);
            return;
        }
        // Usually set to 8, but other values valid too.
        ErrNonfatal("Suspicious offset of first Exif IFD value",0,0);
    }

    DirWithThumbnailPtrs = NULL;


    // First directory starts 16 bytes in.  All offset are relative to 8 bytes in.
    ProcessExifDir(ExifSection+8+FirstOffset, ExifSection+8, length-8, 0);

    ImageInfo.ThumbnailAtEnd = ImageInfo.ThumbnailOffset >= ImageInfo.LargestExifOffset ? TRUE : FALSE;

    if (DumpExifMap){
        unsigned a,b;
        printf("Map: %05d- End of exif\n",length-8);
        for (a=0;a<length-8;a+= 10){
            printf("Map: %05d ",a);
            for (b=0;b<10;b++) printf(" %02x",*(ExifSection+8+a+b));
            printf("\n");
        }
    }


    // Compute the CCD width, in millimeters.
    if (FocalplaneXRes != 0 && ExifImageWidth != 0){
        // Note: With some cameras, its not possible to compute this correctly because
        // they don't adjust the indicated focal plane resolution units when using less
        // than maximum resolution, so the CCDWidth value comes out too small.  Nothing
        // that Jhad can do about it - its a camera problem.
        ImageInfo.CCDWidth = (float)(ExifImageWidth * FocalplaneUnits / FocalplaneXRes);

        if (ImageInfo.FocalLength && ImageInfo.FocalLength35mmEquiv == 0){
            // Compute 35 mm equivalent focal length based on sensor geometry if we haven't
            // already got it explicitly from a tag.
            ImageInfo.FocalLength35mmEquiv = (int)(ImageInfo.FocalLength/ImageInfo.CCDWidth*36 + 0.5);
        }
    } */
}

#pragma endregion

#pragma region JPEG

// auxilliary struct 
typedef struct SectionsData {

    int itemlen;
    int prev;
    int marker;
    int ll,lh, got;
    uchar *Data;

} SectionsData;

// FIXME:
void jpeg_processDQT (SectionsData secs_data) {

    /* int a;
    int c;
    int tableindex, coefindex, row, col;
    unsigned int table[64];
    int *reftable = NULL;
    double cumsf = 0.0, cumsf2 = 0.0;
    int allones = 1;

    a=2; // first two bytes is length
    while (a<length)
    {
        c = Data[a++];
        tableindex = c & 0x0f;
        if (ShowTags>1){
            printf("DQT:  table %d precision %d\n", tableindex, (c>>4) ? 16 : 8);
        }
        if (tableindex < 2){
            reftable = deftabs[tableindex];
        }

        // Read in the table, compute statistics relative to reference table 
        for (coefindex = 0; coefindex < 64; coefindex++) {
            unsigned int val;
            if (c>>4) {
                register unsigned int temp;
                temp=(unsigned int) (Data[a++]);
                temp *= 256;
                val=(unsigned int) Data[a++] + temp;
            } else {
                val=(unsigned int) Data[a++];
            }
            table[coefindex] = val;
            if (reftable) {
                double x;
                // scaling factor in percent 
                x = 100.0 * (double)val / (double)reftable[coefindex];
                cumsf += x;
                cumsf2 += x * x;
                // separate check for all-ones table (Q 100)
                if (val != 1) allones = 0;
            }
        }
        // If requested, print table in normal array order 
        if (ShowTags>2){
            for (row=0; row<8; row++) {
                printf("    ");
                for (col=0; col<8; col++) {
                    printf("%5u ", table[jpeg_zigzag_order[row*8+col]]);
                }
                printf("\n");
            }
        }
        // Print summary stats 
        if (reftable) { // terse output includes quality 
            double qual, var;
            cumsf /= 64.0;    // mean scale factor 
            cumsf2 /= 64.0;
            var = cumsf2 - (cumsf * cumsf); // variance 
            if (allones){      // special case for all-ones table 
                qual = 100.0;
            }else if (cumsf <= 100.0){
                qual = (200.0 - cumsf) / 2.0;
            }else{
                qual = 5000.0 / cumsf;
            }
            if (ShowTags>1)  printf("  ");

            if (ShowTags){
                printf("Approximate quality factor for qtable %d: %.0f (scale %.2f, var %.2f)\n",
                     tableindex, qual, cumsf, var);
            } else {
                if (tableindex == 0){
                    ImageInfo.QualityGuess = (int)(qual+0.5);
                }
            }
        }
    } */
}

// process a COM marker
void jpeg_process_COM (const uchar *data, int length) {}

// process a SOFn marker -> used for image dimensions
void jpeg_process_SOFn (const uchar *data, int marker) {

}

// check sections array to see if it needs to be increased in size
int jpeg_checkAllocatedSections (Cimage *cimage) {

    if (cimage) {
        if (cimage->sectionsRead > cimage->sectionsAllocated) {
            logMsg (stderr, ERROR, NO_TYPE, "Sections allocation screwup!");
            return 1;
        }

        if (cimage->sectionsRead >= cimage->sectionsAllocated) {
            cimage->sectionsAllocated += cimage->sectionsAllocated / 2;
            cimage->sections = (Section *) realloc (cimage->sections, 
                sizeof (Section) * cimage->sectionsAllocated);

            if (!cimage->sections) {
                logMsg (stderr, ERROR, NO_TYPE, "Failed to realloc cimage sections!");
                return 1;
            }
        }

        return 0;
    }

    return 1;

}

int jpeg_readEntireImage (Cimage *cimage) {

    int cp, ep, size;
    // Determine how much file is left.
    cp = ftell (cimage->file);
    fseek (cimage->file, 0, SEEK_END);
    ep = ftell(cimage->file);
    fseek (cimage->file, cp, SEEK_SET);

    size = ep - cp;
    uchar *Data = (uchar *) malloc (size);
    if (!Data) {
        logMsg (stderr, ERROR, IMAGE, "Could not allocate data for entire image!");
        return 1;
    }

    int got = fread (Data, 1, size, cimage->file);
    if (got != size) {
        logMsg (stderr, ERROR, IMAGE, "Could not read the rest of the image!");
        return 1;
    }

    jpeg_checkAllocatedSections (cimage);
    cimage->sections[cimage->sectionsRead].data = Data;
    cimage->sections[cimage->sectionsRead].size = size;
    cimage->sections[cimage->sectionsRead].type = PSEUDO_IMAGE_MARKER;
    cimage->sectionsRead++;

    // HaveAll = 1;

    return 0;

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

void jpeg_processSOFn (Cimage *cimage, SectionsData secs_data) {

    int data_precision, num_components;

    data_precision = secs_data.Data[2];

    cimage->imgData->height = Get16m (secs_data.Data + 3);
    cimage->imgData->width = Get16m (secs_data.Data + 5);

    num_components = secs_data.Data[7];

    if (num_components == 3) cimage->imgData->isColor = 1;
    else cimage->imgData->isColor = 0;

    cimage->imgData->process = secs_data.marker;

    #ifdef CIMAGE_DEBUG
    logMsg (stdout, DEBUG, IMAGE, createString ("JPEG image is %uw * %uh, %d color components, %d bits per sample.",
        cimage->imgData->width, cimage->imgData->height, num_components, data_precision));
    #endif

}

int jpeg_handleSection (Cimage *cimage, SectionsData secs_data) {

    switch (secs_data.marker) {
        // stop before hitting compressed data 
        case M_SOS: 
            // if reading entire image is requested, read the rest of the data
            if (cimage->readmode & READ_IMAGE) 
                return jpeg_readEntireImage (cimage);
            break;

         // used for jpeg quality guessing
        case M_DQT: jpeg_processDQT (secs_data); break;

        // also used for jpeg quality guessing
        case M_DHT: /* FIXME: process_DHT(Data, itemlen); */ break;

        // in case it's a tables-only JPEG stream
        case M_EOI: 
            logMsg (stdout, ERROR, IMAGE, "No image in jpeg!");
            return 1;

        // FIXME:
        // comment section
        case M_COM: 
                /* if (HaveCom || ((ReadMode & READ_METADATA) == 0)){
                    // Discard this section.
                    free(Sections[--SectionsRead].Data);
                }else{
                    process_COM(Data, itemlen);
                    HaveCom = TRUE;
                } */
                break;

        // TODO:
        case M_JFIF: /* jpeg_processJFIF (); */ break;

        // FIXME:
        case M_EXIF:
            // There can be different section using the same marker.
            /* if (ReadMode & READ_METADATA) {
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
            free(Sections[--SectionsRead].Data); */

            if (memcmp (secs_data.Data + 2, "Exif", 4) == 0){
                exif_process (secs_data.Data, secs_data.itemlen);
                break;
            }
            break; 
 
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
            } */
            break;

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
            jpeg_processSOFn (cimage, secs_data);
            break;

        // skip any other section
        default: 
            #ifdef CIMAGE_DEBUG
            logMsg (stdout, WARNING, IMAGE,
                createString ("Jpeg section marker 0x%02x size %d\n", 
                secs_data.marker, secs_data.itemlen));
            #endif
            break;
    }

}

// parse the marker stream until SOS or EOI is seen
int jpeg_readSections (Cimage *cimage) {

    //  int HaveCom = FALSE;

    int a = fgetc (cimage->file);
    if (a != 0xff || fgetc (cimage->file) != M_SOI) return 1;

    cimage->imgData->jfifHeader.xDensity = cimage->imgData->jfifHeader.yDensity = 300;
    cimage->imgData->jfifHeader.resolutionUnits = 1;

    // read jpeg sections based on marker
    SectionsData secs_data;

    for (;;) {
        secs_data.marker = 0;

        if (jpeg_checkAllocatedSections (cimage)) {
            logMsg (stderr, ERROR, IMAGE, "Failed to allocate sections!");
            return 1;
        }

        secs_data.prev = 0;
        for (a = 0; ; a++) {
            secs_data.marker = fgetc (cimage->file);
            if (secs_data.marker != 0xff && secs_data.prev == 0xff) break;
            if (secs_data.marker == EOF) {
                logMsg (stderr, ERROR, IMAGE, "Unexpected end of file -- secs_data.marker");
                return 1;
            }
            secs_data.prev = secs_data.marker;
        }

        // FIXME: err non fatal
        if (a > 10) {
            // logMsg (stderr, ERROR, IMAGE, 
            //     createString ("Extraneous %d padding bytes before section %02X", a - 1, secs_data.marker));
            // return 1;
        }

        cimage->sections[cimage->sectionsRead].type = secs_data.marker;

        // read the length of the section
        secs_data.lh = fgetc (cimage->file);
        secs_data.ll = fgetc (cimage->file);
        if (secs_data.lh == EOF || secs_data.ll == EOF) {
            logMsg (stderr, ERROR, IMAGE, "Unexpected end of file -- secs_data.lh");
            return 1;
        }

        secs_data.itemlen = (secs_data.lh << 8) | secs_data.ll;

         if (secs_data.itemlen < 2) {
            logMsg (stderr, ERROR, IMAGE, "Invalid marker!");
            return 1;
        }

        cimage->sections[cimage->sectionsRead].size = secs_data.itemlen;

        secs_data.Data = (uchar *) malloc (secs_data.itemlen);
        if (!secs_data.Data) {
            logMsg (stderr, ERROR, IMAGE, "Could not allocate memory!");
            return 1;
        }

        cimage->sections[cimage->sectionsRead].data = secs_data.Data;

        // Store first two pre-read bytes.
        secs_data.Data[0] = (uchar) secs_data.lh;
        secs_data.Data[1] = (uchar) secs_data.ll;

        // Read the whole section.
        secs_data.got = fread (secs_data.Data + 2, 1, secs_data.itemlen - 2, cimage->file); 
        if (secs_data.got != secs_data.itemlen - 2) {
            logMsg (stderr, ERROR, IMAGE, "Premature end of file -- secs_data.got");
            return 1;
        }

        cimage->sectionsRead++;

        // now we handle the section
        jpeg_handleSection (cimage, secs_data);

        // FIXME: 27/11/2018 - this is for tetsing
        if (secs_data.marker == M_EXIF) break;

        free (secs_data.Data);
    }

    return 0;   // success

}

int jpeg_readFile (const char *filename, Cimage *cimage) {

    cimage->file = fopen (filename, "rb");
    if (!cimage->file) {
        logMsg (stderr, ERROR, IMAGE, createString ("Failed to open: %s", filename));
        return 1;
    }

    // scan the jpeg headers
    if (!jpeg_readSections (cimage)) {
        logMsg (stdout, DEBUG, IMAGE, "Done getting jpeg sections.");
        fclose (cimage->file);
        return 0;
    }

    else {
        logMsg (stderr, ERROR, IMAGE, "Failed to read jpeg sections!");
        fclose (cimage->file);
        return 1;
    }

    /* else {
        logMsg (stdout, WARNING, IMAGE, createString ("Not jpeg: %s", filename));
        fclose (file);
        return 1;
    } */

}

#pragma endregion

#pragma region IMAGES

// create a new cimage data
Cimage *cimage_new_cimage (ReadMode readmode) {

    Cimage *cimage = (Cimage *) malloc (sizeof (Cimage));
    if (cimage) {
        cimage->sections = (Section *) calloc (5, sizeof (Section));
        cimage->sectionsAllocated = 5;
        cimage->sectionsRead = 0;

        // init the image data structre
        cimage->imgData = (ImageData *) malloc (sizeof (ImageData)); 
        if (cimage->imgData) {
            cimage->imgData->flashUsed = -1;
            cimage->imgData->meteringMode = -1;
            cimage->imgData->whitebalance = -1;
        }

        cimage->readmode = readmode;
    }

    return cimage;

}

// TODO: 14/11/2018 -- are we correctly freing the sections array?
void cimage_del_cimage (Cimage *cimage) {

    if (cimage) {
        if (cimage->sections) free (cimage->sections);
        if (cimage->imgData) free (cimage->imgData);
        free (cimage);
    }

}

// FIXME: print ALL the info
// print ALL the info that we could find in an image
void cimage_print_cimage (Cimage *cimage) {

    if (cimage) {
        printf ("File name    : %s\n", cimage->imgData->filename);
        printf ("File size    : %d bytes\n", cimage->imgData->fileSize);

        // FIXME:
        // char Temp[20];
        // FileTimeAsString(Temp);
        // printf("File date    : %s\n",Temp);

        if (cimage->imgData->cameraMake[0]) {
            printf ("Camera make  : %s\n", cimage->imgData->cameraMake);
            printf ("Camera model : %s\n", cimage->imgData->cameraModel);
        }

        if (cimage->imgData->dateTime[0])
            printf ("Date/Time    : %s\n", cimage->imgData->dateTime);

        printf ("Resolution   : %d x %d\n", cimage->imgData->width, cimage->imgData->height);


        // Only print orientation if one was supplied, and if its not 1 (normal orientation)
        if (cimage->imgData->orientation > 1 && cimage->imgData->orientation <=8)
            printf ("Orientation  : %s\n",  OrientTab[cimage->imgData->orientation]);

        if (cimage->imgData->isColor == 0)
            printf ("Color/bw     : Black and white\n");

        /* if (ImageInfo.FlashUsed >= 0){
            if (ImageInfo.FlashUsed & 1){    
                printf("Flash used   : Yes");
                switch (ImageInfo.FlashUsed){
                    case 0x5: printf(" (Strobe light not detected)"); break;
                    case 0x7: printf(" (Strobe light detected) "); break;
                    case 0x9: printf(" (manual)"); break;
                    case 0xd: printf(" (manual, return light not detected)"); break;
                    case 0xf: printf(" (manual, return light  detected)"); break;
                    case 0x19:printf(" (auto)"); break;
                    case 0x1d:printf(" (auto, return light not detected)"); break;
                    case 0x1f:printf(" (auto, return light detected)"); break;
                    case 0x41:printf(" (red eye reduction mode)"); break;
                    case 0x45:printf(" (red eye reduction mode return light not detected)"); break;
                    case 0x47:printf(" (red eye reduction mode return light  detected)"); break;
                    case 0x49:printf(" (manual, red eye reduction mode)"); break;
                    case 0x4d:printf(" (manual, red eye reduction mode, return light not detected)"); break;
                    case 0x4f:printf(" (red eye reduction mode, return light detected)"); break;
                    case 0x59:printf(" (auto, red eye reduction mode)"); break;
                    case 0x5d:printf(" (auto, red eye reduction mode, return light not detected)"); break;
                    case 0x5f:printf(" (auto, red eye reduction mode, return light detected)"); break;
                }
            }else{
                printf("Flash used   : No");
                switch (ImageInfo.FlashUsed){
                    case 0x18:printf(" (auto)"); break;
                }
            }
            printf("\n");
        }


        if (ImageInfo.FocalLength){
            printf("Focal length : %4.1fmm",(double)ImageInfo.FocalLength);
            if (ImageInfo.FocalLength35mmEquiv){
                printf("  (35mm equivalent: %dmm)", ImageInfo.FocalLength35mmEquiv);
            }
            printf("\n");
        }

        if (ImageInfo.DigitalZoomRatio > 1){
            // Digital zoom used.  Shame on you!
            printf("Digital Zoom : %1.3fx\n", (double)ImageInfo.DigitalZoomRatio);
        }

        if (ImageInfo.CCDWidth){
            printf("CCD width    : %4.2fmm\n",(double)ImageInfo.CCDWidth);
        }

        if (ImageInfo.ExposureTime){
            if (ImageInfo.ExposureTime < 0.010){
                printf("Exposure time: %6.4f s ",(double)ImageInfo.ExposureTime);
            }else{
                printf("Exposure time: %5.3f s ",(double)ImageInfo.ExposureTime);
            }
            if (ImageInfo.ExposureTime <= 0.5){
                printf(" (1/%d)",(int)(0.5 + 1/ImageInfo.ExposureTime));
            }
            printf("\n");
        }
        if (ImageInfo.ApertureFNumber){
            printf("Aperture     : f/%3.1f\n",(double)ImageInfo.ApertureFNumber);
        }
        if (ImageInfo.Distance){
            if (ImageInfo.Distance < 0){
                printf("Focus dist.  : Infinite\n");
            }else{
                printf("Focus dist.  : %4.2fm\n",(double)ImageInfo.Distance);
            }
        }

        if (ImageInfo.ISOequivalent){
            printf("ISO equiv.   : %2d\n",(int)ImageInfo.ISOequivalent);
        }

        if (ImageInfo.ExposureBias){
            // If exposure bias was specified, but set to zero, presumably its no bias at all,
            // so only show it if its nonzero.
            printf("Exposure bias: %4.2f\n",(double)ImageInfo.ExposureBias);
        }
            
        switch(ImageInfo.Whitebalance) {
            case 1:
                printf("Whitebalance : Manual\n");
                break;
            case 0:
                printf("Whitebalance : Auto\n");
                break;
        }

        //Quercus: 17-1-2004 Added LightSource, some cams return this, whitebalance or both
        switch(ImageInfo.LightSource) {
            case 1:
                printf("Light Source : Daylight\n");
                break;
            case 2:
                printf("Light Source : Fluorescent\n");
                break;
            case 3:
                printf("Light Source : Incandescent\n");
                break;
            case 4:
                printf("Light Source : Flash\n");
                break;
            case 9:
                printf("Light Source : Fine weather\n");
                break;
            case 11:
                printf("Light Source : Shade\n");
                break;
            default:; //Quercus: 17-1-2004 There are many more modes for this, check Exif2.2 specs
                // If it just says 'unknown' or we don't know it, then
                // don't bother showing it - it doesn't add any useful information.
        }

        if (ImageInfo.MeteringMode > 0){ // 05-jan-2001 vcs
            printf("Metering Mode: ");
            switch(ImageInfo.MeteringMode) {
            case 1: printf("average\n"); break;
            case 2: printf("center weight\n"); break;
            case 3: printf("spot\n"); break;
            case 4: printf("multi spot\n");  break;
            case 5: printf("pattern\n"); break;
            case 6: printf("partial\n");  break;
            case 255: printf("other\n");  break;
            default: printf("unknown (%d)\n",ImageInfo.MeteringMode); break;
            }
        }

        if (ImageInfo.ExposureProgram){ // 05-jan-2001 vcs
            switch(ImageInfo.ExposureProgram) {
            case 1:
                printf("Exposure     : Manual\n");
                break;
            case 2:
                printf("Exposure     : program (auto)\n");
                break;
            case 3:
                printf("Exposure     : aperture priority (semi-auto)\n");
                break;
            case 4:
                printf("Exposure     : shutter priority (semi-auto)\n");
                break;
            case 5:
                printf("Exposure     : Creative Program (based towards depth of field)\n"); 
                break;
            case 6:
                printf("Exposure     : Action program (based towards fast shutter speed)\n");
                break;
            case 7:
                printf("Exposure     : Portrait Mode\n");
                break;
            case 8:
                printf("Exposure     : LandscapeMode \n");
                break;
            default:
                break;
            }
        }
        switch(ImageInfo.ExposureMode){
            case 0: // Automatic (not worth cluttering up output for)
                break;
            case 1: printf("Exposure Mode: Manual\n");
                break;
            case 2: printf("Exposure Mode: Auto bracketing\n");
                break;
        }

        if (ImageInfo.DistanceRange) {
            printf("Focus range  : ");
            switch(ImageInfo.DistanceRange) {
                case 1:
                    printf("macro");
                    break;
                case 2:
                    printf("close");
                    break;
                case 3:
                    printf("distant");
                    break;
            }
            printf("\n");
        }



        if (ImageInfo.Process != M_SOF0){
            // don't show it if its the plain old boring 'baseline' process, but do
            // show it if its something else, like 'progressive' (used on web sometimes)
            unsigned a;
            for (a=0;;a++){
                if (a >= PROCESS_TABLE_SIZE){
                    // ran off the end of the table.
                    printf("Jpeg process : Unknown\n");
                    break;
                }
                if (ProcessTable[a].Tag == ImageInfo.Process){
                    printf("Jpeg process : %s\n",ProcessTable[a].Desc);
                    break;
                }
            }
        }

        if (ImageInfo.GpsInfoPresent){
            printf("GPS Latitude : %s\n",ImageInfo.GpsLat);
            printf("GPS Longitude: %s\n",ImageInfo.GpsLong);
            if (ImageInfo.GpsAlt[0]) printf("GPS Altitude : %s\n",ImageInfo.GpsAlt);
        }

        if (ImageInfo.QualityGuess){
            printf("JPEG Quality : %d\n", ImageInfo.QualityGuess);
        }

        // Print the comment. Print 'Comment:' for each new line of comment.
        if (ImageInfo.Comments[0]){
            int a,c;
            printf("Comment      : ");
            if (!ImageInfo.CommentWidthchars){
                for (a=0;a<MAX_COMMENT_SIZE;a++){
                    c = ImageInfo.Comments[a];
                    if (c == '\0') break;
                    if (c == '\n'){
                        // Do not start a new line if the string ends with a carriage return.
                        if (ImageInfo.Comments[a+1] != '\0'){
                            printf("\nComment      : ");
                        }else{
                            printf("\n");
                        }
                    }else{
                        putchar(c);
                    }
                }
                printf("\n");
            }else{
                printf("%.*ls\n", ImageInfo.CommentWidthchars, (wchar_t *)ImageInfo.Comments);
            }
        } */
    } 

}

// TODO: just print the must important img data
void cimage_print_concise_cimage (Cimage *cimage) {}

// TODO: 14/11/2018 - maybe we might need a return value?
// do selected operations to one file a time
void cimage_processFile (const char *filename) {

    #ifdef CIMAGE_DEBUG
        logMsg (stdout, DEBUG, NO_TYPE, "Processing file...");
    #endif

    bool modified = false;

    // TODO: how do we select the correct readmode?
    ReadMode readmode = READ_METADATA;

    // start with an empty image data
    Cimage *cimage = cimage_new_cimage (readmode);
    if (!cimage || !cimage->imgData) {
        logMsg (stderr, ERROR, NO_TYPE, "Unable to allocate a new cimage structure!");
        return;
    }
    
    // TODO: store file date/time using fstat
    // thisis the creation day of the file in the current machine

    strncpy (cimage->imgData->filename, filename, 256);

    // TODO: set command line options
    // no command option selected, so just read te img info
    if (!jpeg_readFile (filename, cimage)) {
        // TODO: we have got the image info, what do we do next?
        // TODO: we need to return the image info!!!
        // we can now display it
        cimage_print_cimage (cimage);
    }

    else {
        // TODO: discard data and return NULL
        cimage_del_cimage (cimage);
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
    else cimage_processFile (argv[1]);
          
    return EXIT_SUCCESS;

}

#pragma endregion