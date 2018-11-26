#ifndef MAINPAGE_H_
    #define MAINPAGE_H_
    #include <stdio.h>
    #include <stdlib.h>
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
    #include <stdbool.h>
    #include <SDL2/SDL_ttf.h>
    #include "SDLlib.h"
    #include "UImenu.h"

typedef struct{
    char Path[100];
    SDL_Texture *image;
    SDL_Rect ImageRect;
    bool isShown;
    int index;
    void *Info;
}Image;


typedef struct{
    Image *imagenes;
    int x;
    int y;
}photoSelect_data;

typedef struct{
    char nombre[100];
    char format[100];
    char Date[100];
    char wH[100];
}Info;

    extern void user();
#endif