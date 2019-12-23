// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>
// #include <stdbool.h>

// #include "cimage.h"

// #include "cengine/os.h"
// #include "cengine/types/types.h"

// #include "app/jpeg.h"
// #include "app/exif.h"

// // #include "utils/myUtils.h"
// // #include "utils/log.h"

// // auxilliary struct 
// typedef struct SectionsData {

//     int itemlen;
//     int prev;
//     int marker;
//     int ll,lh, got;
//     uchar *Data;

// } SectionsData;

// static inline int Get16m (const void *Short) { return (((uchar *) Short)[0] << 8) | ((uchar *) Short)[1]; }

// // FIXME:
// void jpeg_processDQT (SectionsData secs_data) {

//     /* int a;
//     int c;
//     int tableindex, coefindex, row, col;
//     unsigned int table[64];
//     int *reftable = NULL;
//     double cumsf = 0.0, cumsf2 = 0.0;
//     int allones = 1;

//     a=2; // first two bytes is length
//     while (a<length)
//     {
//         c = Data[a++];
//         tableindex = c & 0x0f;
//         if (ShowTags>1){
//             printf("DQT:  table %d precision %d\n", tableindex, (c>>4) ? 16 : 8);
//         }
//         if (tableindex < 2){
//             reftable = deftabs[tableindex];
//         }

//         // Read in the table, compute statistics relative to reference table 
//         for (coefindex = 0; coefindex < 64; coefindex++) {
//             unsigned int val;
//             if (c>>4) {
//                 register unsigned int temp;
//                 temp=(unsigned int) (Data[a++]);
//                 temp *= 256;
//                 val=(unsigned int) Data[a++] + temp;
//             } else {
//                 val=(unsigned int) Data[a++];
//             }
//             table[coefindex] = val;
//             if (reftable) {
//                 double x;
//                 // scaling factor in percent 
//                 x = 100.0 * (double)val / (double)reftable[coefindex];
//                 cumsf += x;
//                 cumsf2 += x * x;
//                 // separate check for all-ones table (Q 100)
//                 if (val != 1) allones = 0;
//             }
//         }
//         // If requested, print table in normal array order 
//         if (ShowTags>2){
//             for (row=0; row<8; row++) {
//                 printf("    ");
//                 for (col=0; col<8; col++) {
//                     printf("%5u ", table[jpeg_zigzag_order[row*8+col]]);
//                 }
//                 printf("\n");
//             }
//         }
//         // Print summary stats 
//         if (reftable) { // terse output includes quality 
//             double qual, var;
//             cumsf /= 64.0;    // mean scale factor 
//             cumsf2 /= 64.0;
//             var = cumsf2 - (cumsf * cumsf); // variance 
//             if (allones){      // special case for all-ones table 
//                 qual = 100.0;
//             }else if (cumsf <= 100.0){
//                 qual = (200.0 - cumsf) / 2.0;
//             }else{
//                 qual = 5000.0 / cumsf;
//             }
//             if (ShowTags>1)  printf("  ");

//             if (ShowTags){
//                 printf("Approximate quality factor for qtable %d: %.0f (scale %.2f, var %.2f)\n",
//                      tableindex, qual, cumsf, var);
//             } else {
//                 if (tableindex == 0){
//                     ImageInfo.QualityGuess = (int)(qual+0.5);
//                 }
//             }
//         }
//     } */
// }

// // process a COM marker
// void jpeg_process_COM (const uchar *data, int length) {}

// // process a SOFn marker -> used for image dimensions
// void jpeg_process_SOFn (const uchar *data, int marker) {

// }

// // check sections array to see if it needs to be increased in size
// int jpeg_checkAllocatedSections (Cimage *cimage) {

//     if (cimage) {
//         if (cimage->sectionsRead > cimage->sectionsAllocated) {
//             logMsg (stderr, ERROR, NO_TYPE, "Sections allocation screwup!");
//             return 1;
//         }

//         if (cimage->sectionsRead >= cimage->sectionsAllocated) {
//             cimage->sectionsAllocated += cimage->sectionsAllocated / 2;
//             cimage->sections = (Section *) realloc (cimage->sections, 
//                 sizeof (Section) * cimage->sectionsAllocated);

//             if (!cimage->sections) {
//                 logMsg (stderr, ERROR, NO_TYPE, "Failed to realloc cimage sections!");
//                 return 1;
//             }
//         }

//         return 0;
//     }

//     return 1;

// }

// int jpeg_readEntireImage (Cimage *cimage) {

//     int cp, ep, size;
//     // Determine how much file is left.
//     cp = ftell (cimage->file);
//     fseek (cimage->file, 0, SEEK_END);
//     ep = ftell(cimage->file);
//     fseek (cimage->file, cp, SEEK_SET);

//     size = ep - cp;
//     uchar *Data = (uchar *) malloc (size);
//     if (!Data) {
//         logMsg (stderr, ERROR, IMAGE, "Could not allocate data for entire image!");
//         return 1;
//     }

//     int got = fread (Data, 1, size, cimage->file);
//     if (got != size) {
//         logMsg (stderr, ERROR, IMAGE, "Could not read the rest of the image!");
//         return 1;
//     }

//     jpeg_checkAllocatedSections (cimage);
//     cimage->sections[cimage->sectionsRead].data = Data;
//     cimage->sections[cimage->sectionsRead].size = size;
//     cimage->sections[cimage->sectionsRead].type = PSEUDO_IMAGE_MARKER;
//     cimage->sectionsRead++;

//     // HaveAll = 1;

//     return 0;

// }

// // FIXME:
// void jpeg_processJFIF () {

//     // Regular jpegs always have this tag, exif images have the exif
//     // marker instead, althogh ACDsee will write images with both markers.
//     // this program will re-create this marker on absence of exif marker.
//     // hence no need to keep the copy from the file.

//     /* if (memcmp(Data+2, "JFIF\0",5)){
//         fprintf(stderr,"Header missing JFIF marker\n");
//     }
//     if (itemlen < 16){
//         fprintf(stderr,"Jfif header too short\n");
//         goto ignore;
//     }

//     ImageInfo.JfifHeader.Present = TRUE;
//     ImageInfo.JfifHeader.ResolutionUnits = Data[9];
//     ImageInfo.JfifHeader.XDensity = (Data[10]<<8) | Data[11];
//     ImageInfo.JfifHeader.YDensity = (Data[12]<<8) | Data[13];
//     if (ShowTags){
//         printf("JFIF SOI marker: Units: %d ",ImageInfo.JfifHeader.ResolutionUnits);
//         switch(ImageInfo.JfifHeader.ResolutionUnits){
//             case 0: printf("(aspect ratio)"); break;
//             case 1: printf("(dots per inch)"); break;
//             case 2: printf("(dots per cm)"); break;
//             default: printf("(unknown)"); break;
//         }
//         printf("  X-density=%d Y-density=%d\n",ImageInfo.JfifHeader.XDensity, ImageInfo.JfifHeader.YDensity);

//         if (Data[14] || Data[15]){
//             fprintf(stderr,"Ignoring jfif header thumbnail\n");
//         }
//     }

//     ignore:

//     free(Sections[--SectionsRead].Data); */

// }

// void jpeg_processSOFn (Cimage *cimage, SectionsData secs_data) {

//     int data_precision, num_components;

//     data_precision = secs_data.Data[2];

//     cimage->imgData->height = Get16m (secs_data.Data + 3);
//     cimage->imgData->width = Get16m (secs_data.Data + 5);

//     num_components = secs_data.Data[7];

//     if (num_components == 3) cimage->imgData->isColor = 1;
//     else cimage->imgData->isColor = 0;

//     cimage->imgData->process = secs_data.marker;

//     #ifdef CIMAGE_DEBUG
//     logMsg (stdout, DEBUG, IMAGE, createString ("JPEG image is %uw * %uh, %d color components, %d bits per sample.",
//         cimage->imgData->width, cimage->imgData->height, num_components, data_precision));
//     #endif

// }

// int jpeg_handleSection (Cimage *cimage, SectionsData secs_data) {

//     switch (secs_data.marker) {
//         // stop before hitting compressed data 
//         case M_SOS: 
//             // if reading entire image is requested, read the rest of the data
//             if (cimage->readmode & READ_IMAGE) 
//                 return jpeg_readEntireImage (cimage);
//             break;

//          // used for jpeg quality guessing
//         case M_DQT: jpeg_processDQT (secs_data); break;

//         // also used for jpeg quality guessing
//         case M_DHT: /* FIXME: process_DHT(Data, itemlen); */ break;

//         // in case it's a tables-only JPEG stream
//         case M_EOI: 
//             logMsg (stdout, ERROR, IMAGE, "No image in jpeg!");
//             return 1;

//         // FIXME:
//         // comment section
//         case M_COM: 
//                 /* if (HaveCom || ((ReadMode & READ_METADATA) == 0)){
//                     // Discard this section.
//                     free(Sections[--SectionsRead].Data);
//                 }else{
//                     process_COM(Data, itemlen);
//                     HaveCom = TRUE;
//                 } */
//                 break;

//         // TODO:
//         case M_JFIF: /* jpeg_processJFIF (); */ break;

//         // FIXME:
//         case M_EXIF:
//             // There can be different section using the same marker.
//             /* if (ReadMode & READ_METADATA) {
//                 if (memcmp(Data+2, "Exif", 4) == 0){
//                     process_EXIF(Data, itemlen);
//                     break;
//                 }else if (memcmp(Data+2, "http:", 5) == 0){
//                     Sections[SectionsRead-1].Type = M_XMP; // Change tag for internal purposes.
//                     if (ShowTags){
//                         printf("Image cotains XMP section, %d bytes long\n", itemlen);
//                         if (ShowTags){
//                             ShowXmp(Sections[SectionsRead-1]);
//                         }
//                     }
//                     break;
//                 }
//             }
//             // Oterwise, discard this section.
//             free(Sections[--SectionsRead].Data); */

//             if (memcmp (secs_data.Data + 2, "Exif", 4) == 0){
//                 exif_process (secs_data.Data, secs_data.itemlen);
//                 break;
//             }
//             break; 
 
//         // FIXME:
//         case M_IPTC:
//             /* if (ReadMode & READ_METADATA){
//                 if (ShowTags){
//                     printf("Image cotains IPTC section, %d bytes long\n", itemlen);
//                 }
//                 // Note: We just store the IPTC section.  Its relatively straightforward
//                 // and we don't act on any part of it, so just display it at parse time.
//             }else{
//                 free(Sections[--SectionsRead].Data);
//             } */
//             break;

//         case M_SOF0: 
//         case M_SOF1: 
//         case M_SOF2: 
//         case M_SOF3: 
//         case M_SOF5: 
//         case M_SOF6: 
//         case M_SOF7: 
//         case M_SOF9: 
//         case M_SOF10:
//         case M_SOF11:
//         case M_SOF13:
//         case M_SOF14:
//         case M_SOF15:
//             jpeg_processSOFn (cimage, secs_data);
//             break;

//         // skip any other section
//         default: 
//             #ifdef CIMAGE_DEBUG
//             logMsg (stdout, WARNING, IMAGE,
//                 createString ("Jpeg section marker 0x%02x size %d\n", 
//                 secs_data.marker, secs_data.itemlen));
//             #endif
//             break;
//     }

// }

// // parse the marker stream until SOS or EOI is seen
// int jpeg_readSections (Cimage *cimage) {

//     //  int HaveCom = FALSE;

//     int a = fgetc (cimage->file);
//     if (a != 0xff || fgetc (cimage->file) != M_SOI) return 1;

//     cimage->imgData->jfifHeader.xDensity = cimage->imgData->jfifHeader.yDensity = 300;
//     cimage->imgData->jfifHeader.resolutionUnits = 1;

//     // read jpeg sections based on marker
//     SectionsData secs_data;

//     for (;;) {
//         secs_data.marker = 0;

//         if (jpeg_checkAllocatedSections (cimage)) {
//             logMsg (stderr, ERROR, IMAGE, "Failed to allocate sections!");
//             return 1;
//         }

//         secs_data.prev = 0;
//         for (a = 0; ; a++) {
//             secs_data.marker = fgetc (cimage->file);
//             if (secs_data.marker != 0xff && secs_data.prev == 0xff) break;
//             if (secs_data.marker == EOF) {
//                 logMsg (stderr, ERROR, IMAGE, "Unexpected end of file -- secs_data.marker");
//                 return 1;
//             }
//             secs_data.prev = secs_data.marker;
//         }

//         // FIXME: err non fatal
//         if (a > 10) {
//             // logMsg (stderr, ERROR, IMAGE, 
//             //     createString ("Extraneous %d padding bytes before section %02X", a - 1, secs_data.marker));
//             // return 1;
//         }

//         cimage->sections[cimage->sectionsRead].type = secs_data.marker;

//         // read the length of the section
//         secs_data.lh = fgetc (cimage->file);
//         secs_data.ll = fgetc (cimage->file);
//         if (secs_data.lh == EOF || secs_data.ll == EOF) {
//             logMsg (stderr, ERROR, IMAGE, "Unexpected end of file -- secs_data.lh");
//             return 1;
//         }

//         secs_data.itemlen = (secs_data.lh << 8) | secs_data.ll;

//          if (secs_data.itemlen < 2) {
//             logMsg (stderr, ERROR, IMAGE, "Invalid marker!");
//             return 1;
//         }

//         cimage->sections[cimage->sectionsRead].size = secs_data.itemlen;

//         secs_data.Data = (uchar *) malloc (secs_data.itemlen);
//         if (!secs_data.Data) {
//             logMsg (stderr, ERROR, IMAGE, "Could not allocate memory!");
//             return 1;
//         }

//         cimage->sections[cimage->sectionsRead].data = secs_data.Data;

//         // Store first two pre-read bytes.
//         secs_data.Data[0] = (uchar) secs_data.lh;
//         secs_data.Data[1] = (uchar) secs_data.ll;

//         // Read the whole section.
//         secs_data.got = fread (secs_data.Data + 2, 1, secs_data.itemlen - 2, cimage->file); 
//         if (secs_data.got != secs_data.itemlen - 2) {
//             logMsg (stderr, ERROR, IMAGE, "Premature end of file -- secs_data.got");
//             return 1;
//         }

//         cimage->sectionsRead++;

//         // now we handle the section
//         jpeg_handleSection (cimage, secs_data);

//         // FIXME: 27/11/2018 - this is for tetsing
//         if (secs_data.marker == M_EXIF) break;

//         free (secs_data.Data);
//     }

//     return 0;   // success

// }

// int jpeg_read_file (const char *filename, Cimage *cimage) {

//     cimage->file = fopen (filename, "rb");
//     if (!cimage->file) {
//         logMsg (stderr, ERROR, IMAGE, createString ("Failed to open: %s", filename));
//         return 1;
//     }

//     // scan the jpeg headers
//     if (!jpeg_readSections (cimage)) {
//         logMsg (stdout, DEBUG, IMAGE, "Done getting jpeg sections.");
//         fclose (cimage->file);
//         return 0;
//     }

//     else {
//         logMsg (stderr, ERROR, IMAGE, "Failed to read jpeg sections!");
//         fclose (cimage->file);
//         return 1;
//     }

//     /* else {
//         logMsg (stdout, WARNING, IMAGE, createString ("Not jpeg: %s", filename));
//         fclose (file);
//         return 1;
//     } */

// }