#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

#include "SDLlib.h"
#define dim 100
#define pressedkey key.keysym.sym





int main(void){
    SDL_Init(SDL_INIT_EVERYTHING);
    char text[10];
    bool exit = false, end = false;
    SDL_Window *window = NULL;//principal window
    SDL_Event event;//the event checker
    SDL_Renderer *renderTarget;
    SDL_Texture *texture[100];
    SDL_Surface *example = NULL;
    SDL_Rect drawingRect;//rectangle
    drawingRect = createRectFixed(0,0,200,200);
    //WE will use renderer for better results
    if(SDL_STARTER(&window,&renderTarget,"cimage")==0){
        texture[1] = LoadTexture("./resources/pp.jpg",renderTarget);
        texture[2] = LoadTexture("./resources/Star_Wars.png",renderTarget);
        SDL_SetRenderTarget(renderTarget,texture[0]);
        SDL_SetRenderDrawColor(renderTarget,255,0,0,255);
        SDL_Rect rectangle = createRectFixed(0,0,50,50);
        SDL_RenderFillRect(renderTarget,&rectangle);
        while(!exit){
            while(SDL_PollEvent(&event)!=0){//EVENTS
                if(event.type == SDL_QUIT){
                    exit=true;
                }else if(event.type == SDL_KEYDOWN){//exit out
                    switch(event.pressedkey){
                        case SDLK_ESCAPE:
                            exit = true;
                        break;
                    }
                }else if(event.type == SDL_MOUSEBUTTONDOWN){//mouse events
                    switch(event.button.button){
                        case SDL_BUTTON_LEFT:   
                            // SDL_FreeSurface(windowSurface);
                            // images[0] = images[2];
                            SDL_RenderClear(renderTarget);
                            texture[0]=texture[1];

                        break;
                        case SDL_BUTTON_RIGHT:
                            // SDL_FreeSurface(windowSurface);
                            // images[0] = images[1];
                            SDL_RenderClear(renderTarget);
                            texture[0]=texture[2];
                        break;
                    }
                }else if(event.type == SDL_TEXTINPUT){
                    if(!end){
                        system("clear");
                        strcpy(text,event.text.text);
                        printf("%s",text);
                    }
                }else if(event.type == SDL_KEYDOWN && event.pressedkey  == SDLK_RETURN){
                        end=true;
                }else if(event.type == SDL_KEYDOWN && event.pressedkey == SDLK_BACKSPACE){
                    if(!end && strlen(text)>0){
                        text[strlen(text)-1]='\0';
                    }
                }
                
            }
            SDL_RenderClear(renderTarget);
            SDL_RenderCopy(renderTarget,texture[0],NULL,NULL);
            SDL_RenderPresent(renderTarget);
        }
        /*for(int i=0; i<dim; i++){
            SDL_FreeSurface(images[i]);
            images[i]=NULL;
        }*/
        SDL_DestroyWindow(window);
        SDL_DestroyTexture(texture[0]);
        SDL_DestroyRenderer(renderTarget);
        SDL_Quit();
    }
} 