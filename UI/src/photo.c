#include "photo.h"

extern bool imageShown;

void openPhoto(Image imagen){
    Info *name = (Info*)imagen.Info;
    int w,h;
    SDL_QueryTexture(imagen.image,NULL,NULL,&w,&h);
    SDL_Log("here\nw:%d\nh:%d",w,h);
    if(w>SCREEN_WIDTH){
        w=SCREEN_WIDTH-100;
    }
    if(h>SCREEN_HEIGHT){
        h = SCREEN_HEIGHT-100;
    }

    SDL_Rect fixedSize;
    fixedSize.x = fixedSize.y = 0;
    SDL_Window *photo;
    SDL_Renderer *renderPhoto;
    SDL_STARTER_FIXED(&photo,&renderPhoto,"PHOTO",h,w);
    bool running=true;
    SDL_Event e;
    if(w>SCREEN_WIDTH){
        fixedSize.w = SCREEN_WIDTH-100;
    }else{
        fixedSize.x  = (SCREEN_WIDTH - w)/2;
        fixedSize.w = w;
    }
    if(h>SCREEN_HEIGHT){
        fixedSize.h = SCREEN_HEIGHT-100;
    }else{
        fixedSize.h = h;
        fixedSize.y  = (SCREEN_HEIGHT- h )/2;
    }
    SDL_Texture *shownPhoto = LoadTexture(imagen.Path,renderPhoto);
    while(running){
        while(SDL_PollEvent(&e)!=0){
            if(e.type == SDL_QUIT){
                running = false;
                imageShown = false;
            }else if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE){
                running = false;
                imageShown = false;
                SDL_Delay(200);
            }
        }
        
        SDL_RenderClear(renderPhoto);
        SDL_Log("Rect: x=%d y=%d w=%d h=%d\n",fixedSize.x,fixedSize.y,fixedSize.w,fixedSize.h);
        SDL_RenderCopy(renderPhoto,shownPhoto,NULL,&fixedSize);
        SDL_RenderPresent(renderPhoto);
    }
    SDL_Log("0Here'snot");
    SDL_DestroyWindow(photo);
        SDL_Log("1Here'snot");
    SDL_DestroyTexture(shownPhoto);
        SDL_Log("2Here'snot");
    SDL_DestroyRenderer(renderPhoto);
        SDL_Log("3Here'snot");
    imageShown = false;
}
