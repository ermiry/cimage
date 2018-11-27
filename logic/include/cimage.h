#ifndef CIMAGE_H
#define CIMAGE_H

#include <stdio.h>

#define CIMAGE_VERSION      "0.1"

#define EXIT_SUCCESS        0
#define EXIT_FAILURE        1

typedef unsigned char uchar;

typedef enum ReadMode {

    READ_METADATA = 1,
    READ_IMAGE = 2,
    READ_ALL = 3,
    READ_ANY = 5  

} ReadMode;

// stores a jpeg file section
typedef struct Section {

    uchar *data;
    int type;
    unsigned size;

} Section;

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

// TODO: what else do wa want to store here?
// maybe the readmode?
// 14/11/2018 -- we create a structure for each image data and sections
typedef struct Cimage {

    Section *sections;      // image's exif sections
    int sectionsAllocated;
    int sectionsRead;

    ImageData *imgData;     // extracted image data

    ReadMode readmode;

} Cimage;

#pragma region JPEG

/*** JPEG marker codes of interest **/

#define M_SOF0  0xC0          // Start Of Frame N
#define M_SOF1  0xC1          // N indicates which compression process
#define M_SOF2  0xC2          // Only SOF0-SOF2 are now in common use
#define M_SOF3  0xC3
#define M_SOF5  0xC5          // NB: codes C4 and CC are NOT SOF markers
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_SOI   0xD8          // Start Of Image (beginning of datastream)
#define M_EOI   0xD9          // End Of Image (end of datastream)
#define M_SOS   0xDA          // Start Of Scan (begins compressed data)
#define M_JFIF  0xE0          // Jfif marker
#define M_EXIF  0xE1          // Exif marker.  Also used for XMP data!
#define M_XMP   0x10E1        // Not a real tag (same value in file as Exif!)
#define M_COM   0xFE          // COMment 
#define M_DQT   0xDB          // Define Quantization Table
#define M_DHT   0xC4          // Define Huffmann Table
#define M_DRI   0xDD
#define M_IPTC  0xED          // IPTC marker

#pragma endregion

#pragma region EXIF

static const char * OrientTab[9] = {
    "Undefined",
    "Normal",           // 1
    "flip horizontal",  // left right reversed mirror
    "rotate 180",       // 3
    "flip vertical",    // upside down mirror
    "transpose",        // Flipped about top-left <--> bottom-right axis.
    "rotate 90",        // rotate 90 cw to right it.
    "transverse",       // flipped about top-right <--> bottom-left axis
    "rotate 270",       // rotate 270 to right it.
};

#pragma endregion

#endif