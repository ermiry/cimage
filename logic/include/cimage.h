#ifndef CIMAGE_H
#define CIMAGE_H

#include <stdio.h>

#define CIMAGE_VERSION      "0.1"

#define EXIT_SUCCESS        0
#define EXIT_FAILURE        1

typedef enum ReadMode {

    READ_METADATA = 1,

} ReadMode;

// info in the jfif header
// this info is not used much, but just in case...
typedef struct JfifHeader {

    char present;
    char resolutionUnits;
    short xDensity;
    short yDensity;

} JfifHeader;

// all the possible info that can be stored inside an exif header
typedef struct ImageData {

    // TODO: 08/11/2018 -- do we wan this here?
    char filename[256];
    // time_t FileDateTime;
    unsigned int fileSize;

    JfifHeader jfifHeader;

    char cameraMake[64];
    char cameraModel[64];
    char dateTime[20];
    unsigned int height, width;
    int orientation;
    int isColor;
    int process;
    int flashUsed;
    float focalLength;
    float exposureTime;
    float apertureFNumber;
    float distance;
    float CCDWidth;
    float exposureBias;
    float digitalZoomRatio;
    int focalLength35mmEquiv; // Exif 2.2 tag - usually not present.
    int whitebalance;
    int meteringMode;
    int exposureProgram;
    int exposureMode;
    int ISOequivalent;
    int lightSource;
    int distanceRange;

    float xResolution;
    float yResolution;
    int resolutionUnit;

    // TODO:
    // char  Comments[MAX_COMMENT_SIZE];
    // int   CommentWidthchars; // If nonzero, widechar comment, indicates number of chars.

    unsigned ThumbnailOffset;          // Exif offset to thumbnail
    unsigned ThumbnailSize;            // Size of thumbnail.
    unsigned LargestExifOffset;        // Last exif data referenced (to check if thumbnail is at end)

    char  ThumbnailAtEnd;              // Exif header ends with the thumbnail
                                       // (we can only modify the thumbnail if its at the end)
    int   ThumbnailSizeOffset;

    // TODO:
    // int  DateTimeOffsets[MAX_DATE_COPIES];
    // int  numDateTimeTags;

    int GpsInfoPresent;
    char GpsLat[31];
    char GpsLong[31];
    char GpsAlt[20];

    int qualityGuess;

} ImageData;

#endif