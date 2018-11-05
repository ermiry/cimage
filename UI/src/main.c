#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL_ttf.h>
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

uint32_t SDL_menu(SDL_Renderer **renderTarget, TTF_Font *Sans, SDL_Color Black,Color color){
    SDL_Rect menu;
    SDL_Rect display;
    SDL_Surface *text; 
    Color rectColor,blackColor;
    rectColor.r=rectColor.g=rectColor.b=100;
    blackColor.r=blackColor.g=blackColor.b=0;
    rectColor.a=blackColor.a=200;
    SDL_Texture *texture;
    char messages[5][100]={"Archivo","Edicion","Insertar","Herramientas","Ayuda"};
    for(int i=0; i<5; i++){
        text = TTF_RenderText_Solid(Sans,messages[i],Black);
        texture = SDL_CreateTextureFromSurface(*renderTarget,text);
        
        menu.h = 55;
        menu.w = 70;
        menu.x = i*70;
        menu.y = 0;
        display.h=menu.h-10;
        display.w = menu.w -10;
        display.x = i*70+5;
        display.y = 8;
        SDL_RenderColor(renderTarget,rectColor);
        SDL_RenderDrawRect(*renderTarget,&menu);
        SDL_RenderColor(renderTarget,blackColor);
        SDL_RenderCopy(*renderTarget,texture,NULL, &display);
    }
    SDL_RenderColor(renderTarget,color);
    return 1;
}

void User(){
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    SDL_Window *window = NULL;
    SDL_Renderer *renderTarget = NULL;
    SDL_Event event;
    SDL_Surface *surface = NULL;
    SDL_Texture *texture[100];
    Color color,colorLine;
    TTF_Font *Sans = TTF_OpenFont(ResourcePath"/Fonts/OpenSans-Light.ttf",12);
    TTF_Font *Roboto = TTF_OpenFont(ResourcePath"/Fonts/Roboto-Regular.ttf",150);
    SDL_Color White = {255,255,255};
    SDL_Color Black = {0,0,0};
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(Sans,"Put your text here",Black);
    if(!surfaceMessage){
        printf("Error creating a surface message %s", SDL_GetError());
    }
    color.a = colorLine.a = 255;
    bool exit=false;
    if(SDL_STARTER_FIXED(&window,&renderTarget,"CIMAGE",1280,720)==0){
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderTarget,surfaceMessage);
        if(!textTexture){
            printf("Error creating a texture message, %s", SDL_GetError());
        }
        SDL_SetWindowResizable(window,true);
        texture[0]=LoadTexture(ResourcePath"/CIMAGE10.png",renderTarget);
        while(!exit){
            while(SDL_PollEvent(&event)!=0){
                if(event.type==SDL_QUIT || (event.type ==SDL_KEYDOWN && event.pressedkey==SDLK_ESCAPE)){
                    exit = true;
                }
            }
            color.r=color.g=color.b=230;
            colorLine.r = colorLine.g = colorLine.b = 120; 
            SDL_RenderClear(renderTarget);
            SDL_RenderColor(&renderTarget,color);
            SDL_Print(&renderTarget,texture[0],1280-138,0,128,128);
            //SDL_Print(&renderTarget,textTexture,0,10,25,115);
            SDL_RenderColor(&renderTarget,colorLine);
            SDL_RenderDrawLine_Gross(&renderTarget,0,1280,120,120,2);
            SDL_menu(&renderTarget,Roboto,Black,color);
            
            SDL_RenderColor(&renderTarget,color);
            SDL_RenderPresent(renderTarget);
        }
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderTarget);
        SDL_Quit();
        
    }
}

int main(void){
    uint32_t cont=0;
    //cont  = ChargingScreen();
    if(cont==-1){
        fprintf(stdout,"Exit program\n");
    }else{
        User();
    }
    return 0;
} 

