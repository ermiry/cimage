#ifndef CIMAGE_H
#define CIMAGE_H

#include <stdio.h>

#define CIMAGE_VERSION      "0.1"

#define EXIT_SUCCESS        0
#define EXIT_FAILURE        1

#define FPS_LIMIT   30

#define DEFAULT_SCREEN_WIDTH    1280    
#define DEFAULT_SCREEN_HEIGHT   720

#define THREAD_OK   0

#ifdef DEV
    #define ASSETS_PATH "./assets/"
#elif PRODUCTION
    #define ASSETS_PATH "../assets/"
#else 
    #define ASSETS_PATH "./assets/"
#endif

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

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

    FILE *file;

} Cimage;

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

extern bool running;
extern bool inGame;
extern bool wasInGame;

extern void quit (int code);
extern void die (const char *error);

#endif