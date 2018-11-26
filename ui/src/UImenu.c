#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include "list.h"
#include <SDL2/SDL_ttf.h>
#include "UImenu.h"


void menuCreator(SDL_Renderer **main_renderer,DoubleList **menu,menuPos pos[5], TTF_Font* Roboto, SDL_Color black){
    int i;

    SDL_Surface *aux=NULL;
    *menu = dlist_init(free);
    menuData *data;
    menuData *option;
    char messages[6][100]={"File\0","Edit\0","Insert\0","View\0","Help\0",'\0'};
    char Opt[5][20][20]={{"Open Folder","New Folder","Preferences","", "", "", "",'\0'},
                        {"Copy","Cut","Paste",'\0'},
                        {"//NOT YET","//NOT YET","//NOT YET","//NOT YET",'\0'},
                        {"FullScreen","View As frames", "List",'\0'},
                        {"Ask","See manual",'\0'}};
    for( i=0; messages[i][0]!='\0'; i++){
        data = (menuData * ) malloc (sizeof(menuData));
        strcpy(data->dataTitle,messages[i]);
        aux = TTF_RenderText_Solid(Roboto,data->dataTitle,black);
        if(!aux){
            printf("Error creating text texture:\n%s\n",SDL_GetError());
        }
        data->text = SDL_CreateTextureFromSurface(*main_renderer,aux);
        if(!data->text){
            printf("Error copying texture to List\n%s",SDL_GetError());
        }
        aux=NULL;
        data->option = dlist_init(free);
        for(int j = 0;  Opt[i][j][0]!='\0'; j++){
            option = (menuData *)malloc(sizeof(menuData));
            strcpy(option->dataTitle , Opt[i][j]);
            printf("%s\n",Opt[i][j]);
            aux = TTF_RenderText_Solid(Roboto,option->dataTitle,black);
            if(!aux){
                printf("%s\n",SDL_GetError());
            }
            option->text = SDL_CreateTextureFromSurface(*main_renderer,aux);
            if(!option->text){
                printf("Error copying texture to List\n%s",SDL_GetError());
            }
            aux=NULL;
            option->option = NULL;
            if(data->option->start){
                dlist_insert_after(data->option,LIST_END(data->option),option);
            }else{
                dlist_insert_after(data->option,LIST_START(data->option),option);
            }
        } 
        if((*menu)->start){                
            dlist_insert_after(*menu, LIST_END(*menu),data);
        }else{
            dlist_insert_after(*menu,LIST_START(*menu),data);
        }
        data->display.x = 55 * i;
        data->display.y = 0;
        data->display.h = 25;
        data->display.w = 40;
        data->select.x = data->display.x -10;
        data->select.y = data->display.y;
        data->select.w = data->display.w + 20;
        data->select.h = data->display.h;
        if(pos[0].x == '\0'){
            pos[i].x = data->display.x;
            pos[i].xF = data->display.x + data->display.w;
            pos[i].y = data->display.y;
            pos[i].yF = data->display.h;
        }
    }    
}

void menuInmenus(SDL_Renderer **main_renderer, int numSelected,SDL_Texture **MenuTexture, TTF_Font *Roboto,menuPos pos[5],DoubleList **menu){
    int x,y;
    SDL_Rect MenuIn;
    SDL_Rect MenuInParam;
    ListElement *e = LIST_START(*menu);
    for(int i=0; i<numSelected; i++){
        e = e->next;
    }
    menuData *h = (menuData *)e->data;
    if(!h){
        printf("error");
    }
    DoubleList *f =h->option;
    ListElement *g = LIST_START(f);
    menuData *k= NULL;
    x = pos[numSelected].x;
    MenuInParam.x = MenuIn.x = x;
    MenuInParam.y = MenuIn.y = 25;
    MenuIn.h = 100;
    MenuIn.w = 300;
    MenuInParam.w = 80;
    MenuInParam.h = 25;
    SDL_SetRenderDrawColor(*main_renderer,250,250,250,200);
    SDL_RenderFillRect(*main_renderer,&MenuIn);
    SDL_SetRenderDrawColor(*main_renderer,220,220,220,255);
    while(g){
        k = (menuData *)g->data;
        SDL_RenderCopy(*main_renderer,k->text,NULL,&MenuInParam);
        MenuInParam.y +=25;
        g=g->next;
    }
}

void menuPrint(SDL_Renderer **main_renderer, bool Selected, int numSelected,SDL_Texture **MenuTexture, TTF_Font *Roboto, menuPos pos[5],DoubleList **menu){
    int i;
    SDL_Surface *text =NULL;
    SDL_Color blue,black={0,0,0};
    SDL_Rect MenuIn;
    TTF_Font *x = TTF_OpenFont("./resources/Fonts/Roboto-Regular.ttf",80);
    blue.r = 97;
    blue.g = 193;
    blue.b = 215;
    blue.a = 255;
    black.r=black.g=black.b=0;
    black.a= 255;
    if(!*menu){
        printf("I created the menu\n");
        menuCreator(main_renderer,menu,pos,x,black);
    }
    ListElement *e = LIST_START(*menu);
    if(!*MenuTexture){
        printf("\nI created a new Menu Texture Target \n");
        //BLANK
        //FIXME: AM I CREATING THE TEXTURE CORRECTLY?
        *MenuTexture = SDL_CreateTexture(*main_renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_STREAMING,1280,720);
        if(!MenuTexture){
            printf("Error Creating Menu Texture, %s\n",SDL_GetError());
        }
        SDL_SetRenderTarget(*main_renderer,*MenuTexture);
        for(i=0; !e; i++,e = e->next){
            menuData *h = (menuData *) e->data;
            SDL_RenderCopy(*main_renderer,h->text,NULL,&h->display);
            printf("%s\n",SDL_GetError());
        }
        SDL_SetRenderTarget(*main_renderer,NULL);
    }else{
        //printf("I updated the Menu Texture Target\n");
        SDL_SetRenderTarget(*main_renderer,*MenuTexture);
        if(Selected) {
            menuData *h = NULL;
            i=0;
            while(e){
                h = (menuData * ) e->data;
                if(i==numSelected){
                    SDL_SetRenderDrawColor(*main_renderer,blue.r,blue.g,blue.b,blue.a);
                    menuInmenus(main_renderer,numSelected,MenuTexture,Roboto,pos,menu);

                }else{
                    SDL_SetRenderDrawColor(*main_renderer,220,220,220,255);
                }
                SDL_RenderFillRect(*main_renderer,&h->display);
                SDL_RenderCopy(*main_renderer,h->text,NULL,&h->display);
                SDL_SetRenderDrawColor(*main_renderer,220,220,220,255);
                e = e->next;
                i++;
            }
        }else{
            menuData *h = NULL;
            while(e){
                h= (menuData *) e->data;
                SDL_SetRenderDrawColor(*main_renderer,220,220,220,255);
                SDL_RenderFillRect(*main_renderer,&h->display);
                SDL_RenderCopy(*main_renderer,h->text,NULL,&h->display);
                e = e->next;
            }
        }
        //SDL_SetRenderDrawColor(*main_renderer,220,220,220,255);
        SDL_SetRenderTarget(*main_renderer,NULL);

    }
}