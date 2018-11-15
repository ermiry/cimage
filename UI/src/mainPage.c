#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <SDL2/SDL_ttf.h>
#include "SDLlib.h"
#include "UImenu.h"
#include "mainPage.h"


typedef struct{
    float w;
    float h;
}porcentage;

typedef struct{
    char Path[100];
    SDL_Texture *image;
    SDL_Rect ImageRect;
    int index;
    void *Info;
    int x;
    int y;
}Image;


void imageCharger(Image *imagenes){
    
}

void imagePrinting(Image *imagenes, SDL_Renderer *main_renderer, SDL_Texture **imageTexture){
    SDL_SetRenderTarget(main_renderer,*imageTexture);
    for(int i=0; i<(sizeof(imagenes)/sizeof(Image)); i++){

    }
}


void scrollBarPrint(SDL_Rect scrollBar, SDL_Rect scrollBarPos, SDL_Texture **scrollBarTexture, SDL_Renderer *main_renderer){
    SDL_SetRenderTarget(main_renderer,*scrollBarTexture);
    SDL_SetRenderDrawColor(main_renderer,170,170,170,180);
    SDL_RenderFillRect(main_renderer,&scrollBar);
    SDL_SetRenderDrawColor(main_renderer,140,140,140,200);
    SDL_RenderFillRect(main_renderer,&scrollBarPos);
    SDL_SetRenderDrawColor(main_renderer,220,220,220,255);
    SDL_SetRenderTarget(main_renderer,NULL);
}

void user(){
    TTF_Init();
    SDL_DisplayMode current;
    SDL_Init(SDL_INIT_VIDEO);
    porcentage por;
    Image *imagenes = (Image *)malloc(sizeof(Image)*20);
    for(int i = 0 ; i<SDL_GetNumVideoDisplays(); i++){
        int should_be_zero = SDL_GetCurrentDisplayMode(i,&current);
        if(should_be_zero !=0){
            SDL_Log("Could not get display mode for video display #%d: %s",i,SDL_GetError());
        }else{
            SDL_Log("Display #%d: current display mode is %dx%dpx @ %dhz.",
                i, current.w, current.h, current.refresh_rate);
            break;
        }
    }
    current.w = SCREEN_WIDTH / current.w;
    current.h = SCREEN_HEIGHT / current.h;
    printf("Porcentage in x:%d -- Porcentage in y:%d\n",current.w,current.h);
    Tiempo Time;
    Time.deltaTime=Time.frameTime=Time.currentTime=Time.prevTime=0;
    SDL_Window *window = NULL;
    SDL_Renderer *main_renderer = NULL;
    SDL_Texture *menuTexture = NULL, *scrollBarTexture = NULL, *imageTexture = NULL;
    SDL_Rect logoRect,scrollBar,scrollBarPos;
    scrollBar.x = scrollBarPos.x  =SCREEN_WIDTH - 20;
    scrollBar.y = scrollBarPos.y = 130;
    scrollBar.w = scrollBarPos.w = 20;
    scrollBar.h = SCREEN_HEIGHT - 125;
    scrollBarPos.h = 50;
    logoRect.x = 1280-90;
    logoRect.y = 20;
    logoRect.w = 90;
    logoRect.h = 90;
    bool running = true,showMenu = false, Selected = false,scroll = false,scrollHang = false, fullScreen = false;
    int numSelected = -1, newY=0;
    DoubleList *menu = NULL;
    TTF_Font *Roboto = TTF_OpenFont("./resources/Fonts/Roboto-Regular.ttf",210);
    if(!Roboto){
        printf("Error Creating Font\n%s",TTF_GetError());
    }
    SDL_Event e;
    menuPos pos[5];
    if(!SDL_STARTER_FIXED(&window,&main_renderer,"CIMAGE",SCREEN_HEIGHT,SCREEN_WIDTH)){
        imageTexture= SDL_CreateTexture(main_renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_STREAMING,SCREEN_WIDTH,SCREEN_WIDTH);
        SDL_Texture *logo = LoadTexture("./resources/CIMAGE10.png",main_renderer);
        scrollBarTexture = SDL_CreateTexture(main_renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_STREAMING,SCREEN_WIDTH,SCREEN_HEIGHT);
        SDL_Texture *mainTexture = NULL;
        while(running){
            SDL_TIME(&Time.prevTime,&Time.currentTime,&Time.deltaTime);
            while(SDL_PollEvent(&e)!=0){
                if(e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.pressedKey == SDLK_ESCAPE)){
                    running = false;
                }else if(e.type == SDL_KEYDOWN && (e.pressedKey == SDLK_LALT || e.pressedKey == SDLK_RALT)){
                    showMenu = !showMenu;
                }else if(e.type == SDL_MOUSEWHEEL){
                    if(e.wheel.y < 0 && scrollBarPos.y <=720-7){
                        scrollBarPos.y += 7;
                        newY += 7;
                        scroll = true;
                    }else if(e.wheel.y >0 && scrollBarPos.y >=137){
                        scrollBarPos.y -= 7;
                        newY  -= 7;
                        scroll = true;
                    }
                }
                else if(e.type == SDL_MOUSEBUTTONUP && scrollHang){
                    scrollHang = false;
                }else if(e.type == SDL_MOUSEBUTTONDOWN  && e.button.button == leftClick && mouseX>1260 && mouseY>130){
                    scrollHang =true;

                }else if(e.type == SDL_KEYDOWN && e.pressedKey == SDLK_F11){
                    SDL_fullScreenToggle(window,&fullScreen);
                } 
                
                if(scrollHang){
                    SDL_GetMouseState(NULL,&scrollBarPos.y);
                    SDL_GetMouseState(NULL,&newY);
                }
                
                //MENU EVENTS
                if(showMenu){
                    if(e.type == click && mouseY<=25){
                        for(int i=0; i<5; i++){
                            if(mouseX >= pos[i].x-10 && mouseX <= pos[i].xF+10){
                                Selected = !Selected;
                                numSelected = i;
                                break;
                            }
                        }
                    }else if(e.type == click && mouseY >20 && Selected){
                        Selected = !Selected;
                        numSelected = -1;
                    }
                }
            }
            Time.frameTime += Time.deltaTime;
            SDL_RenderClear(main_renderer);
            SDL_SetRenderDrawColor(main_renderer,220,220,220,255);
            if(showMenu){
                menuPrint(&main_renderer,Selected,numSelected,&menuTexture,Roboto,pos,&menu);
            }
            scrollBarPrint(scrollBar,scrollBarPos,&scrollBarTexture,main_renderer);
            SDL_RenderCopy(main_renderer,logo,NULL,&logoRect);
            SDL_SetRenderDrawColor(main_renderer,100,100,100,255);
            SDL_RenderDrawLine_Gross(&main_renderer,0,1280,125,125,5);
            SDL_SetRenderDrawColor(main_renderer,220,220,220,255);
            SDL_RenderPresent(main_renderer); 
            SDL_Delay(5);  
        }
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(main_renderer);
        SDL_Quit();
    }
}