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
