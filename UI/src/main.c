#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define red "\033[1;31m"
#define yellow "\033[0;32m"
#define reset "\033[0m"

int main(void){
    SDL_Window *window = NULL;
    SDL_Surface *imageSurface = NULL;
    SDL_Surface *windowSurface = NULL;
    SDL_CreateWindow("CIMAGE",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,500,500,SDL_WINDOW_SHOWN);
    if(!window){
        fprintf(stderr,"%s[ERR]ERROR CREATING WINDOW%s\n"red,reset);
    }else{
        //created Window
        windowSurface = SDL_GetWindowSurface(window);
        imageSurface = IMG_Load("./resources/pp.jpg");
        if(!imageSurface){
            fprintf(stderr,"%s[ERR]ERROR LOADING IMAGE%s\n",red,reset);
        }else{
            SDL_UpdateWindowSurface(window);
            SDL_BlitSurface(imageSurface,NULL,windowSurface,NULL);
        }
    }
    SDL_UpdateWindowSurface(window);
    SDL_Delay(20000);
    SDL_FreeSurface(imageSurface);
    imageSurface =  NULL;
    SDL_DestroyWindow(window);
    window = NULL;
}