#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <time.h>
#include <stdbool.h>
#include "SDLlib.h"



int SDL_STARTER(SDL_Window **window,SDL_Renderer **renderer, char title[100]){
    /**
     * Initialize SDL, creates a window and asign a window surface
    */
    *window = SDL_CreateWindow(title,center,center,HEIGHT,WIDTH,SDL_WINDOW_SHOWN);
    *renderer = SDL_CreateRenderer(*window,-1,0);
    if(!*window){
        fprintf(stderr,"[ERR]ERROR CREATING WINDOW\n");
        return -1;
    }else{
        return 0;
    }
}

void clean(SDL_Window **window, SDL_Renderer **renderer){
    SDL_DestroyWindow(*window);
    
    SDL_DestroyRenderer(*renderer);
    SDL_Quit();
}

int SDL_STARTER_FIXED(SDL_Window **window,SDL_Renderer **renderer, char title[100],int h,int w){
    SDL_RendererInfo info;
    char titulo[256] = {0};
    int r_has_texture_support = 0;
    *window = SDL_CreateWindow(title,center,center,w,h,SDL_WINDOW_SHOWN);
    SDL_GetWindowSize(*window,&w,&h);
    *renderer = SDL_CreateRenderer(*window,-1,SDL_RENDERER_ACCELERATED);
    //SDL_RenderSetIntegerScale(*renderer,true);
    SDL_RenderSetLogicalSize(*renderer,SCREEN_WIDTH,SCREEN_HEIGHT);
    SDL_GetRendererInfo(*renderer,&info);
    r_has_texture_support = info.flags & SDL_RENDERER_TARGETTEXTURE;
    SDL_Log("Renderer %s started.",info.name);
    SDL_SetWindowResizable(*window,true);
    SDL_strlcat(titulo,info.name,sizeof(titulo));
    if(!r_has_texture_support){
        SDL_Log("Renderer has no target texture support");
        return -1;
    }
     if(!*window){
        fprintf(stderr,"[ERR]ERROR CREATING WINDOW\n");
        return -1;
    }else{
        return 0;
    }
}

void IMG_Loader(SDL_Surface **image, char PATH[100]){
    
    *image = IMG_Load(PATH);//loads an image
}

void SDL_exit(SDL_Event event, bool *exit){
    while(SDL_PollEvent(&event) != 0){
        if(event.type == SDL_QUIT){
            *exit=true;
        }else if(event.type == SDL_KEYDOWN){
            switch(event.pressedkey){
                case SDLK_ESCAPE:
                    *exit = true;
                break;
            }
        }
    }
}

SDL_Rect createRect(int x, int y){
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = WIDTH;
    rect.h = HEIGHT;
    return rect;
}

SDL_Rect createRectFixed(int x,int y, int w,int h){
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    return rect;
}

SDL_Texture *LoadTexture(char filePath[100],SDL_Renderer *renderTarget){
    SDL_Texture *texture = NULL;
    SDL_Surface *surface = NULL;                                                                         
    IMG_Loader(&surface,filePath);
    if(!surface){
        printf("ERROR\n");
    }
    texture = SDL_CreateTextureFromSurface(renderTarget,surface);
    if(!texture){
        printf("ERROR TEXTURE, %s\n",SDL_GetError());
    }   
    SDL_FreeSurface(surface);
    return texture;
}

void SDL_TIME(int *prevTime,int *currentTime, float *deltaTime){
    *prevTime = *currentTime;
    *currentTime = SDL_GetTicks();
    *deltaTime = (*currentTime - *prevTime)/1000.0f;
}

void SDL_Circle(SDL_Renderer *renderTarget, int r, int h, int k){
    Circle circle;
    circle.r = r;
    circle.h = h;
    circle.k = k;
    circle.nX=circle.nY=circle.oX=circle.oY=0;
    circle.step = (3.14 * 2)/50;
    circle.oX = circle.h + (circle.r *cos(0));
    circle.oY = circle.k - (circle.r * sin(0));
    for(float i=0; i<(3.14 * 2); i+=circle.step){
        circle.nX = circle.h + (circle.r * cos(i));
        circle.nY = circle.k - (circle.r * sin(i));
        SDL_RenderDrawLine(renderTarget,circle.oX, circle.oY,circle.nX,circle.nY);
        circle.oX = circle.nX;
        circle.oY = circle.nY;
    }
}

void SDL_FilledCircle(SDL_Renderer *renderTarget,int r,int h, int k){
    for(float i=r; i>=0; i-=0.1){
        SDL_Circle(renderTarget,i,h,k);
    }
}

void SDL_RandomColor(SDL_Renderer *renderTarget, Color color){
    srand(time(NULL));
    color.r=rand()%30;
    color.g=rand()%147;
    color.b=rand()%54;
    SDL_SetRenderDrawColor(renderTarget,color.r,color.g,color.b,color.a);
}

void SDL_RenderColor(SDL_Renderer **renderTarget, Color color){
    SDL_SetRenderDrawColor(*renderTarget,color.r,color.g,color.b,color.a);
}

void SDL_RenderDrawLine_Gross(SDL_Renderer **renderTarget, int x1, int x2, int y1, int y2, int thickness){
    for(int i=0; i<thickness; i++){
        SDL_RenderDrawLine(*renderTarget,x1,y1+i,x2,y2+i);
    }
}

void SDL_fullScreenToggle(SDL_Window *window, bool *FullScreen){
    *FullScreen = !*FullScreen;
    if(*FullScreen){
        SDL_SetWindowFullscreen(window,SDL_WINDOW_FULLSCREEN_DESKTOP);
    }else{
        SDL_SetWindowFullscreen(window,0);
    }
}