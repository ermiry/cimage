#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include "SDLlib.h"

#define pressedkey key.keysym.sym


int main(void){
    bool exit=false;
    SDL_Window *window = NULL;
    SDL_Event event;
    SDL_Surface *imageSurface = NULL;
    SDL_Surface *windowSurface = NULL;
    if(SDL_STARTER(&window,&windowSurface,"cimage")==0){
        windowSurface = SDL_GetWindowSurface(window);
        IMG_Loader(&imageSurface,"./resources/pp.jpg");
        while(!exit){
            SDL_PRINT(&imageSurface,&windowSurface,&window);
            while(SDL_PollEvent(&event)!=0){
                if(event.type == SDL_QUIT){
                    exit=true;
                }else if(event.type == SDL_KEYDOWN){
                    switch(event.pressedkey){
                        case SDLK_1:
                            exit = true;
                        break;
                        case SDLK_a:
                            printf("Presionaste la A");
                            break;
                    }
                }
            }
        }
        
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
}