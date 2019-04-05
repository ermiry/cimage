#ifndef UIMENU_H_
    #define UIMENU_H_
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
    #include <SDL2/SDL_audio.h>
    #include <stdbool.h>
    #include <math.h>
    #include <unistd.h>
    #include "list.h"
    #include <SDL2/SDL_ttf.h>

    typedef struct menuData{
        char dataTitle[100];
        SDL_Texture * text;
        DoubleList *option;
        SDL_Rect select;
        SDL_Rect display;
    }menuData;

    typedef struct{
        int x;
        int xF;
        int y;
        int yF;
    }menuPos;

    extern void menuCreator(SDL_Renderer **main_renderer,DoubleList **menu,menuPos pos[5], TTF_Font* Roboto, SDL_Color black);
    extern void menuPrint(SDL_Renderer **main_renderer, bool Selected, int numSelected,SDL_Texture **MenuTexture, TTF_Font *Roboto, menuPos pos[5], DoubleList **menu);

#endif