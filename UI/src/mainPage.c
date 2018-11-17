#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <SDL2/SDL_ttf.h>
#include <pthread.h>
#include "SDLlib.h"
#include "UImenu.h"
#include "mainPage.h"
#include "UIChargingScreen.h"

bool loading = false, imagesAreCharging = false;

typedef struct{
    float w;
    float h;
}porcentage;

typedef struct{
    char Path[100];
    SDL_Texture *image;
    SDL_Rect ImageRect;
    bool isShown;
    int index;
    void *Info;
}Image;

typedef struct thread_data{
    SDL_Renderer *renderer_t;
    Image *imagenes;
    int number;
}th_data;


void imageCharger(Image *imagenes,SDL_Renderer *main_renderer){
    int x = 1, y = 1;

    for(int i=0; i<100; i++){
        imagenes[i].index = i;
        imagenes[i].ImageRect.x = (110 * x) - 60;
        imagenes[i].ImageRect.y = (140 * y);
        imagenes[i].ImageRect.w = 100;
        imagenes[i].ImageRect.h = 80;
        char *b = (char *)  malloc(sizeof(char) * 100);
        if(i<=20){
            strcpy(imagenes[i].Path,"./resources/rectp.png");
            strcpy(b,"I'm a red photo");
        }else{
            strcpy(imagenes[i].Path,"./resources/rectg.png");
            strcpy(b,"I'm a green photo");
        }
        if(imagenes[i].ImageRect.y>70 && imagenes[i].ImageRect.y<765){
            imagenes[i].isShown = true;
            imagenes[i].image = LoadTexture(imagenes[i].Path,main_renderer);
        }else{
            imagenes[i].isShown = false;
            imagenes[i].image = NULL;
        }

        //imagenes[i].image = LoadTexture(imagenes[i].Path,main_renderer);
        imagenes[i].Info = b;
        x++;
        if(x==12){
            y++;
            x=1;
        }
    }
    loading = false;
}

void ImageMover(Image *imagenes, int Ymove){
    for(int i=0; i<100; i++){
        imagenes[i].ImageRect.y += Ymove;
    }
}

int imageLoader(void* data){
    th_data *data_s = data;
    SDL_Renderer *main_renderer = data_s ->renderer_t;
    for(int i=data_s->number * 12; i<(data_s -> number *12) + 12; i++){
        if(!(data_s->imagenes[i].image)){
            data_s->imagenes[i].image = LoadTexture(data_s->imagenes[i].Path,main_renderer);
            printf("Index Picture: %d --Texture Created\n",data_s->imagenes[i].index);
        }
        data_s->imagenes[i].isShown = true;
    }
    imagesAreCharging = false;
}


void imageCleaner(Image *imagenes){
    if(imagenes->image){
        SDL_DestroyTexture(imagenes->image);
        printf("Index Picture: %d --Texture Destroyed\n",imagenes->index);
        imagenes->image=NULL;
    }
    imagenes->isShown = false;
}
//TODO: To make an efficient way to work, We use threads, to charge and render gray rects while
//the real photo is charging
void imagePrinting(Image *imagenes, SDL_Renderer **main_renderer, SDL_Texture **imageTexture, SDL_Thread *ch_t){
    th_data *data_t = (th_data*)malloc(sizeof(th_data));
    int iInThreading=-1;
    SDL_SetRenderTarget(*main_renderer,*imageTexture);
    for(int i=0; i<10; i++){
        char *b = (char *)imagenes[i].Info;
        //printf("Info photo with index:%d -- %s\n",imagenes[i].index,b);
        if(imagenes[i*12].ImageRect.y>70 && imagenes[i*12].ImageRect.y<775){
            //This line should enter just once per cycle and only if new information is charged
            if(imagenes[i*12].isShown==false){
                SDL_SetRenderDrawColor(*main_renderer,120,120,120,255);
                data_t->imagenes = imagenes;
                data_t->renderer_t = *main_renderer;
                for(int j=0; j<12; j++){
                    SDL_RenderFillRect(*main_renderer,&imagenes[i*12 + j].ImageRect);
                }
                iInThreading = i;
                data_t->number = iInThreading;
                ch_t = SDL_CreateThread(imageLoader,"Loading Images",data_t);
                
            }
            //TODO:I dont Access to this one if a thread in this line is running
            for(int j=0; j<12 && iInThreading!=i; j++){
                SDL_RenderCopy(*main_renderer,imagenes[i*12 +j].image,NULL,&imagenes[i*12 +j].ImageRect);
            }
        }else{
            imageCleaner(&imagenes[i]);
        }
    }
    SDL_SetRenderTarget(*main_renderer,NULL);
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
    void *res;
    SDL_DisplayMode current;
    SDL_Init(SDL_INIT_VIDEO);
    Image *imagenes = (Image *)malloc(sizeof(Image)*120);
    current = currentWindow();
    Tiempo Time;
    Time.deltaTime=Time.frameTime=Time.currentTime=Time.prevTime=0;
    SDL_Window *window = NULL;
    SDL_Renderer *main_renderer = NULL;
    SDL_TextureAccess access = SDL_TEXTUREACCESS_TARGET;
    SDL_Texture *menuTexture = NULL, *scrollBarTexture = NULL, *imageTexture = NULL;
    SDL_Rect logoRect,scrollBar,scrollBarPos, upBar;
    scrollBar.x = scrollBarPos.x  =SCREEN_WIDTH - 20;
    scrollBar.y = scrollBarPos.y = 130;
    scrollBar.w = scrollBarPos.w = 20;
    scrollBar.h = SCREEN_HEIGHT - 125;
    scrollBarPos.h = 50;
    logoRect.x = 1280-90;
    logoRect.y = 10;
    logoRect.w = logoRect.h = 90;
    upBar.x = upBar.y = 0;
    upBar.w = SCREEN_WIDTH;
    upBar.h = 124;
    bool running = true,showMenu = false, Selected = false,scrollHang = false;
    bool fullScreen = false;
    int numSelected = -1, newY=0,s;
    DoubleList *menu = NULL;
    TTF_Font *Roboto = TTF_OpenFont("./resources/Fonts/Roboto-Regular.ttf",210);
    SDL_Event e;
    menuPos pos[5];
    SDL_Thread *ChargeThread, *ch_t;
    if(!SDL_STARTER_FIXED(&window,&main_renderer,"CIMAGE",SCREEN_HEIGHT,SCREEN_WIDTH)){
        imageTexture= SDL_CreateTexture(main_renderer,SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING  ,SCREEN_WIDTH,SCREEN_WIDTH);
        if(!imageTexture){
            printf("%s\n",SDL_GetError());
        }
        SDL_Texture *logo = LoadTexture("./resources/CIMAGE10.png",main_renderer);
        scrollBarTexture = SDL_CreateTexture(main_renderer,SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,scrollBar.w,scrollBar.h);
        SDL_Texture *mainTexture = NULL;
        SDL_Color white = {255,255,255,255};
        loading = true;
        ChargeThread = SDL_CreateThread(ChargingScreen,"Carga de pantalla",NULL);
        imageCharger(imagenes,main_renderer);
        if(!loading){
            //SDL_WaitThread(ChargeThread,0);
            SDL_DetachThread(ChargeThread);
        }
        while(running){
            SDL_TIME(&Time.prevTime,&Time.currentTime,&Time.deltaTime);
            while(SDL_PollEvent(&e)!=0){
                if(e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.pressedKey == SDLK_ESCAPE)){
                    running = false;
                }else if(e.type == SDL_KEYDOWN && (e.pressedKey == SDLK_LALT ||
                    e.pressedKey == SDLK_RALT)){
                    showMenu = !showMenu;
                }else if(e.type == SDL_MOUSEWHEEL){
                    if(e.wheel.y < 0 && scrollBarPos.y <=720-7){
                        scrollBarPos.y += 7;
                        newY += 7;
                        ImageMover(imagenes,-7);
                    }else if(e.wheel.y >0 && scrollBarPos.y >=137){
                        scrollBarPos.y -= 7;
                        newY  -= 7;
                        ImageMover(imagenes,+7);
                    }
                }
                else if(e.type == SDL_MOUSEBUTTONUP && scrollHang){
                    scrollHang = false;
                }else if(e.type == SDL_MOUSEBUTTONDOWN  && e.button.button == leftClick && 
                    mouseX>1260 && mouseY>130){
                    scrollHang =true;

                }else if(e.type == SDL_KEYDOWN && e.pressedKey == SDLK_F11){
                    SDL_fullScreenToggle(window,&fullScreen);
                } 
                
                if(scrollHang){
                    newY = scrollBarPos.y;
                    SDL_GetMouseState(NULL,&scrollBarPos.y);
                    newY = -scrollBarPos.y + newY;
                    ImageMover(imagenes,newY);  
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
            imagePrinting(imagenes,&main_renderer,&imageTexture, ch_t);
            SDL_SetRenderDrawColor(main_renderer,100,100,100,255);
            SDL_RenderDrawLine_Gross(&main_renderer,0,1280,125,125,5);
            SDL_SetRenderDrawColor(main_renderer,220,220,220,255);
            SDL_RenderFillRect(main_renderer,&upBar);
            SDL_RenderCopy(main_renderer,logo,NULL,&logoRect);
            SDL_RenderPresent(main_renderer); 
            SDL_Delay(5);  
        }
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(main_renderer);
        SDL_Quit();
    }
}