#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <SDL2/SDL_ttf.h>
#include "SDLlib.h"
#include "UImenu.h"
#include "mainPage.h"


void user(){
    TTF_Init();
    Tiempo Time;
    Time.deltaTime=Time.frameTime=Time.currentTime=Time.prevTime=0;
    SDL_Window *window = NULL;
    SDL_Renderer *main_renderer = NULL;
    SDL_Texture *menuTexture = NULL;
    SDL_Rect logoRect;
    logoRect.x = 1280-90;
    logoRect.y = 20;
    logoRect.w = 90;
    logoRect.h = 90;
    bool running = true,showMenu = false, Selected = false;
    int numSelected = -1;
    DoubleList *menu = NULL;
    TTF_Font *Roboto = TTF_OpenFont("./resources/Fonts/Roboto-Regular.ttf",210);
    if(!Roboto){
        printf("Error Creating Font\n%s",TTF_GetError());
    }
    SDL_Event e;
    menuPos pos[5];
    if(!SDL_STARTER_FIXED(&window,&main_renderer,"CIMAGE",SCREEN_HEIGHT,SCREEN_WIDTH)){
        SDL_Texture *logo = LoadTexture("./resources/CIMAGE10.png",main_renderer);
        SDL_Texture *mainTexture = NULL;
        while(running){
            SDL_TIME(&Time.prevTime,&Time.currentTime,&Time.deltaTime);
            while(SDL_PollEvent(&e)!=0){
                if(e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.pressedKey == SDLK_ESCAPE)){
                    running = false;
                }else if(e.type == SDL_KEYDOWN && (e.pressedKey == SDLK_LALT || e.pressedKey == SDLK_RALT)){
                    printf("Menu Shown\n");
                    showMenu = !showMenu;
                }else if(showMenu){
                    if(e.type == click && mouseY<=25){
                        printf("You clicked in the menu\n");
                        printf("Mouse in X:%d, Mouse in Y:%d",mouseX,mouseY);
                        for(int i=0; i<5; i++){
                            printf("X inicial:%d, X final:%d\n",pos[i].x,pos[i].xF);
                            if(mouseX >= pos[i].x-10 && mouseX <= pos[i].xF+10){
                                Selected = !Selected;
                                numSelected = i;
                                printf("You clicked in %d element\n",i);
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
                ListElement *e = LIST_START(menu);
                menuData *h = (menuData *) e->data;
                //SDL_RenderCopy(main_renderer,h->text,NULL,&h->display);
                if(!menuTexture && Time.frameTime>=1){
                    printf("%.2f It didnt printed anything, %s\n",Time.frameTime,SDL_GetError());
                    Time.frameTime = 0;
                }
            }

            SDL_RenderCopy(main_renderer,logo,NULL,&logoRect);
            SDL_SetRenderDrawColor(main_renderer,100,100,100,255);
            SDL_RenderDrawLine_Gross(&main_renderer,0,1280,125,125,5);
            SDL_SetRenderDrawColor(main_renderer,220,220,220,255);
            SDL_RenderPresent(main_renderer);   
        }
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(main_renderer);
        SDL_Quit();
    }
}