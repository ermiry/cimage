#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <math.h>

#include "SDLlib.h"
#define dim 100
#define pressedkey key.keysym.sym
#define ResourcePath "./resources/"
#define  click SDL_MOUSEBUTTONDOWN
#define  leftClick SDL_BUTTON_LEFT
#define rightClick SDL_BUTTON_RIGHT 
#define PI 3.1416
#define RAD PI/180




uint32_t ChargingScreen(){
    SDL_Init(SDL_INIT_EVERYTHING);
    bool exit = false, forced = false;;
    Sprite sprite;
    int xAux,yAux,cont=0,cont2=0,ff=1,contAux=0,MaxCont=0,aux,MAX = 8;
    Tiempo Time;
    Color color;
    color.a=255;

    //SDL_VARIABLES
    SDL_Window *window = NULL;//principal window
    SDL_Event event;//the event checker
    SDL_Renderer *renderTarget;
    SDL_Texture *texture[2];
    //WE will use renderer for better results
    if(SDL_STARTER(&window,&renderTarget,"cimage")==0){
        texture[0] = LoadTexture(ResourcePath"/CIMAGE33.png",renderTarget);
        texture[1] = LoadTexture(ResourcePath"/background.jpg",renderTarget);
        //game cycle
        while(!exit){
            while(SDL_PollEvent(&event)!=0){//EVENTS
                if(event.type == SDL_QUIT){// || (event.type == SDL_KEYDOWN && event.pressedkey==SDLK_ESCAPE)){
                    exit=true;
                    forced = true;
                }
            }
            /*
            *Render Printing
            * */
            SDL_RenderClear(renderTarget);
            SDL_RenderCopy(renderTarget,texture[1],NULL,NULL);
            SDL_RenderCopy(renderTarget,texture[0],NULL,NULL);
        
            for(int i=cont2; i<cont; i++){//charging rullete
                aux = (360/MAX);
                xAux = 20 * cos((i*aux)*RAD);
                yAux = -20 * sin((i*aux)*RAD);
                if(i%4==0){
                    //SDL_RandomColor(renderTarget,color);
                }
                SDL_SetRenderDrawColor(renderTarget,255,255,255,255);
                SDL_FilledCircle(renderTarget,4,HEIGHT/2 + xAux, WIDTH/2 + 160 + yAux);
            }
            SDL_RenderPresent(renderTarget);
            contAux++;
            if(contAux>32){
                if(cont<MAX){
                    cont++;
                }else{
                    if(cont2<MAX){
                        cont2++;
                    }else{
                        cont2=0;
                        cont=0;
                    }
                    
                }
                    contAux=0;
                    MaxCont++;
            }
            if(MaxCont==2*MAX){
                exit=true;
            }
            
        }
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderTarget);
        SDL_Quit();
        if(forced){
            return -1;
        }else{
            return 0;
        }
    }
}

void User(){
    SDL_Window *window = NULL;
    SDL_Renderer *renderTarget = NULL;
    SDL_Event event;
    SDL_Texture *texture[100];
    Color color;
    color.a = 255;
    bool exit=false;
    if(SDL_STARTER_FIXED(&window,&renderTarget,"CIMAGE",1280,720)==0){
        texture[0]=LoadTexture(ResourcePath"/CIMAGE10.png",renderTarget);
        while(!exit){
            while(SDL_PollEvent(&event)!=0){
                if(event.type==SDL_QUIT || (event.type ==SDL_KEYDOWN && event.pressedkey==SDLK_ESCAPE)){
                    exit = true;
                }
            }
            color.r=color.g=color.b=230;
            SDL_RenderClear(renderTarget);
            SDL_RenderCopy(renderTarget,texture[0],NULL,NULL);
            SDL_SetRenderDrawColor(renderTarget,color.r,color.g,color.b,color.a);
            SDL_RenderPresent(renderTarget);
        }
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderTarget);
        SDL_Quit();
    }
}

int main(void){
    uint32_t cont=0;
    cont  = ChargingScreen();
    if(cont==-1){
        fprintf(stdout,"Exit program\n");
    }else{
        User();
    }
    return 0;
} 