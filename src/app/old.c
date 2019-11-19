// // create a new cimage data
// Cimage *cimage_new (ReadMode readmode) {

//     Cimage *cimage = (Cimage *) malloc (sizeof (Cimage));
//     if (cimage) {
//         cimage->sections = (Section *) calloc (5, sizeof (Section));
//         cimage->sectionsAllocated = 5;
//         cimage->sectionsRead = 0;

//         // init the image data structre
//         cimage->imgData = (ImageData *) malloc (sizeof (ImageData)); 
//         if (cimage->imgData) {
//             cimage->imgData->flashUsed = -1;
//             cimage->imgData->meteringMode = -1;
//             cimage->imgData->whitebalance = -1;
//         }

//         cimage->readmode = readmode;
//     }

//     return cimage;

// }

// // TODO: 14/11/2018 -- are we correctly freing the sections array?
// void cimage_delete (Cimage *cimage) {

//     if (cimage) {
//         if (cimage->sections) free (cimage->sections);
//         if (cimage->imgData) free (cimage->imgData);
//         free (cimage);
//     }

// }

// // FIXME: print ALL the info
// // print ALL the info that we could find in an image
// void cimage_print_cimage (Cimage *cimage) {

//     if (cimage) {
//         printf ("File name    : %s\n", cimage->imgData->filename);
//         printf ("File size    : %d bytes\n", cimage->imgData->fileSize);

//         // FIXME:
//         // char Temp[20];
//         // FileTimeAsString(Temp);
//         // printf("File date    : %s\n",Temp);

//         if (cimage->imgData->cameraMake[0]) {
//             printf ("Camera make  : %s\n", cimage->imgData->cameraMake);
//             printf ("Camera model : %s\n", cimage->imgData->cameraModel);
//         }

//         if (cimage->imgData->dateTime[0])
//             printf ("Date/Time    : %s\n", cimage->imgData->dateTime);

//         printf ("Resolution   : %d x %d\n", cimage->imgData->width, cimage->imgData->height);


//         // Only print orientation if one was supplied, and if its not 1 (normal orientation)
//         if (cimage->imgData->orientation > 1 && cimage->imgData->orientation <=8)
//             printf ("Orientation  : %s\n",  OrientTab[cimage->imgData->orientation]);

//         if (cimage->imgData->isColor == 0)
//             printf ("Color/bw     : Black and white\n");

//         /* if (ImageInfo.FlashUsed >= 0){
//             if (ImageInfo.FlashUsed & 1){    
//                 printf("Flash used   : Yes");
//                 switch (ImageInfo.FlashUsed){
//                     case 0x5: printf(" (Strobe light not detected)"); break;
//                     case 0x7: printf(" (Strobe light detected) "); break;
//                     case 0x9: printf(" (manual)"); break;
//                     case 0xd: printf(" (manual, return light not detected)"); break;
//                     case 0xf: printf(" (manual, return light  detected)"); break;
//                     case 0x19:printf(" (auto)"); break;
//                     case 0x1d:printf(" (auto, return light not detected)"); break;
//                     case 0x1f:printf(" (auto, return light detected)"); break;
//                     case 0x41:printf(" (red eye reduction mode)"); break;
//                     case 0x45:printf(" (red eye reduction mode return light not detected)"); break;
//                     case 0x47:printf(" (red eye reduction mode return light  detected)"); break;
//                     case 0x49:printf(" (manual, red eye reduction mode)"); break;
//                     case 0x4d:printf(" (manual, red eye reduction mode, return light not detected)"); break;
//                     case 0x4f:printf(" (red eye reduction mode, return light detected)"); break;
//                     case 0x59:printf(" (auto, red eye reduction mode)"); break;
//                     case 0x5d:printf(" (auto, red eye reduction mode, return light not detected)"); break;
//                     case 0x5f:printf(" (auto, red eye reduction mode, return light detected)"); break;
//                 }
//             }else{
//                 printf("Flash used   : No");
//                 switch (ImageInfo.FlashUsed){
//                     case 0x18:printf(" (auto)"); break;
//                 }
//             }
//             printf("\n");
//         }


//         if (ImageInfo.FocalLength){
//             printf("Focal length : %4.1fmm",(double)ImageInfo.FocalLength);
//             if (ImageInfo.FocalLength35mmEquiv){
//                 printf("  (35mm equivalent: %dmm)", ImageInfo.FocalLength35mmEquiv);
//             }
//             printf("\n");
//         }

//         if (ImageInfo.DigitalZoomRatio > 1){
//             // Digital zoom used.  Shame on you!
//             printf("Digital Zoom : %1.3fx\n", (double)ImageInfo.DigitalZoomRatio);
//         }

//         if (ImageInfo.CCDWidth){
//             printf("CCD width    : %4.2fmm\n",(double)ImageInfo.CCDWidth);
//         }

//         if (ImageInfo.ExposureTime){
//             if (ImageInfo.ExposureTime < 0.010){
//                 printf("Exposure time: %6.4f s ",(double)ImageInfo.ExposureTime);
//             }else{
//                 printf("Exposure time: %5.3f s ",(double)ImageInfo.ExposureTime);
//             }
//             if (ImageInfo.ExposureTime <= 0.5){
//                 printf(" (1/%d)",(int)(0.5 + 1/ImageInfo.ExposureTime));
//             }
//             printf("\n");
//         }
//         if (ImageInfo.ApertureFNumber){
//             printf("Aperture     : f/%3.1f\n",(double)ImageInfo.ApertureFNumber);
//         }
//         if (ImageInfo.Distance){
//             if (ImageInfo.Distance < 0){
//                 printf("Focus dist.  : Infinite\n");
//             }else{
//                 printf("Focus dist.  : %4.2fm\n",(double)ImageInfo.Distance);
//             }
//         }

//         if (ImageInfo.ISOequivalent){
//             printf("ISO equiv.   : %2d\n",(int)ImageInfo.ISOequivalent);
//         }

//         if (ImageInfo.ExposureBias){
//             // If exposure bias was specified, but set to zero, presumably its no bias at all,
//             // so only show it if its nonzero.
//             printf("Exposure bias: %4.2f\n",(double)ImageInfo.ExposureBias);
//         }
            
//         switch(ImageInfo.Whitebalance) {
//             case 1:
//                 printf("Whitebalance : Manual\n");
//                 break;
//             case 0:
//                 printf("Whitebalance : Auto\n");
//                 break;
//         }

//         //Quercus: 17-1-2004 Added LightSource, some cams return this, whitebalance or both
//         switch(ImageInfo.LightSource) {
//             case 1:
//                 printf("Light Source : Daylight\n");
//                 break;
//             case 2:
//                 printf("Light Source : Fluorescent\n");
//                 break;
//             case 3:
//                 printf("Light Source : Incandescent\n");
//                 break;
//             case 4:
//                 printf("Light Source : Flash\n");
//                 break;
//             case 9:
//                 printf("Light Source : Fine weather\n");
//                 break;
//             case 11:
//                 printf("Light Source : Shade\n");
//                 break;
//             default:; //Quercus: 17-1-2004 There are many more modes for this, check Exif2.2 specs
//                 // If it just says 'unknown' or we don't know it, then
//                 // don't bother showing it - it doesn't add any useful information.
//         }

//         if (ImageInfo.MeteringMode > 0){ // 05-jan-2001 vcs
//             printf("Metering Mode: ");
//             switch(ImageInfo.MeteringMode) {
//             case 1: printf("average\n"); break;
//             case 2: printf("center weight\n"); break;
//             case 3: printf("spot\n"); break;
//             case 4: printf("multi spot\n");  break;
//             case 5: printf("pattern\n"); break;
//             case 6: printf("partial\n");  break;
//             case 255: printf("other\n");  break;
//             default: printf("unknown (%d)\n",ImageInfo.MeteringMode); break;
//             }
//         }

//         if (ImageInfo.ExposureProgram){ // 05-jan-2001 vcs
//             switch(ImageInfo.ExposureProgram) {
//             case 1:
//                 printf("Exposure     : Manual\n");
//                 break;
//             case 2:
//                 printf("Exposure     : program (auto)\n");
//                 break;
//             case 3:
//                 printf("Exposure     : aperture priority (semi-auto)\n");
//                 break;
//             case 4:
//                 printf("Exposure     : shutter priority (semi-auto)\n");
//                 break;
//             case 5:
//                 printf("Exposure     : Creative Program (based towards depth of field)\n"); 
//                 break;
//             case 6:
//                 printf("Exposure     : Action program (based towards fast shutter speed)\n");
//                 break;
//             case 7:
//                 printf("Exposure     : Portrait Mode\n");
//                 break;
//             case 8:
//                 printf("Exposure     : LandscapeMode \n");
//                 break;
//             default:
//                 break;
//             }
//         }
//         switch(ImageInfo.ExposureMode){
//             case 0: // Automatic (not worth cluttering up output for)
//                 break;
//             case 1: printf("Exposure Mode: Manual\n");
//                 break;
//             case 2: printf("Exposure Mode: Auto bracketing\n");
//                 break;
//         }

//         if (ImageInfo.DistanceRange) {
//             printf("Focus range  : ");
//             switch(ImageInfo.DistanceRange) {
//                 case 1:
//                     printf("macro");
//                     break;
//                 case 2:
//                     printf("close");
//                     break;
//                 case 3:
//                     printf("distant");
//                     break;
//             }
//             printf("\n");
//         }



//         if (ImageInfo.Process != M_SOF0){
//             // don't show it if its the plain old boring 'baseline' process, but do
//             // show it if its something else, like 'progressive' (used on web sometimes)
//             unsigned a;
//             for (a=0;;a++){
//                 if (a >= PROCESS_TABLE_SIZE){
//                     // ran off the end of the table.
//                     printf("Jpeg process : Unknown\n");
//                     break;
//                 }
//                 if (ProcessTable[a].Tag == ImageInfo.Process){
//                     printf("Jpeg process : %s\n",ProcessTable[a].Desc);
//                     break;
//                 }
//             }
//         }

//         if (ImageInfo.GpsInfoPresent){
//             printf("GPS Latitude : %s\n",ImageInfo.GpsLat);
//             printf("GPS Longitude: %s\n",ImageInfo.GpsLong);
//             if (ImageInfo.GpsAlt[0]) printf("GPS Altitude : %s\n",ImageInfo.GpsAlt);
//         }

//         if (ImageInfo.QualityGuess){
//             printf("JPEG Quality : %d\n", ImageInfo.QualityGuess);
//         }

//         // Print the comment. Print 'Comment:' for each new line of comment.
//         if (ImageInfo.Comments[0]){
//             int a,c;
//             printf("Comment      : ");
//             if (!ImageInfo.CommentWidthchars){
//                 for (a=0;a<MAX_COMMENT_SIZE;a++){
//                     c = ImageInfo.Comments[a];
//                     if (c == '\0') break;
//                     if (c == '\n'){
//                         // Do not start a new line if the string ends with a carriage return.
//                         if (ImageInfo.Comments[a+1] != '\0'){
//                             printf("\nComment      : ");
//                         }else{
//                             printf("\n");
//                         }
//                     }else{
//                         putchar(c);
//                     }
//                 }
//                 printf("\n");
//             }else{
//                 printf("%.*ls\n", ImageInfo.CommentWidthchars, (wchar_t *)ImageInfo.Comments);
//             }
//         } */
//     } 

// }

// // TODO: just print the must important img data
// void cimage_print_concise_cimage (Cimage *cimage) {}

// // TODO: 14/11/2018 - maybe we might need a return value?
// // do selected operations to one file a time
// void cimage_processFile (const char *filename) {

//     // #ifdef CIMAGE_DEBUG
//     //     logMsg (stdout, DEBUG, NO_TYPE, "Processing file...");
//     // #endif

//     bool modified = false;

//     // TODO: how do we select the correct readmode?
//     ReadMode readmode = READ_METADATA;

//     // start with an empty image data
//     Cimage *cimage = cimage_new (readmode);
//     if (!cimage || !cimage->imgData) {
//         // FIXME:
//         // logMsg (stderr, ERROR, NO_TYPE, "Unable to allocate a new cimage structure!");
//         return;
//     }
    
//     // TODO: store file date/time using fstat
//     // thisis the creation day of the file in the current machine

//     strncpy (cimage->imgData->filename, filename, 256);

//     // TODO: set command line options
//     // no command option selected, so just read te img info
//     if (!jpeg_read_file (filename, cimage)) {
//         // TODO: we have got the image info, what do we do next?
//         // TODO: we need to return the image info!!!
//         // we can now display it
//         cimage_print_cimage (cimage);
//     }

//     else {
//         // TODO: discard data and return NULL
//         cimage_delete (cimage);
//     }

// }