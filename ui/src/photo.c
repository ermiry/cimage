#include "photo.h"

extern bool imageShown;
bool infOpened = false;


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
            }else if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_i && !infOpened){
                infOpened = !infOpened;
                SDL_Delay(1000);
            }
        }
        
        if(infOpened){
             TTF_Font *robotoInfo = TTF_OpenFont("./resources/Fonts/Roboto-Medium.ttf",255);
            SDL_Color color = {250,250,250,255};
            SDL_Surface *text = NULL, *textInPhoto = NULL;
            SDL_Rect inf, inf2;
            inf.h = 30;
            inf.w = 70;
            inf.x = 5;
            inf.y = 500;
            inf2.h = 30;
            inf.w = 70;
            inf.x = 155;
            inf.y = 500;
            Info *photoInfo = (Info*) imagen.Info;
            text = TTF_RenderText_Solid(robotoInfo,"Name",color);
            SDL_RenderCopy(renderPhoto,SDL_CreateTextureFromSurface(renderPhoto,text),NULL,&inf);
            textInPhoto = TTF_RenderText_Solid(robotoInfo,photoInfo->nombre,color);
            SDL_RenderCopy(renderPhoto,SDL_CreateTextureFromSurface(renderPhoto,textInPhoto),NULL,&inf2);
            SDL_FreeSurface(text);
            SDL_FreeSurface(textInPhoto);
            text = TTF_RenderText_Solid(robotoInfo,"Date",color);
            inf.y = inf2.y = 80 + 490;
            SDL_RenderCopy(renderPhoto,SDL_CreateTextureFromSurface(renderPhoto,text),NULL,&inf);
            textInPhoto = TTF_RenderText_Solid(robotoInfo,photoInfo->nombre,color);
            SDL_RenderCopy(renderPhoto,SDL_CreateTextureFromSurface(renderPhoto,textInPhoto),NULL,&inf2);
            SDL_FreeSurface(text);
            SDL_FreeSurface(textInPhoto);
            text = TTF_RenderText_Solid(robotoInfo,"Format",color);
            inf.y = inf2.y = 140 + 490;
            SDL_RenderCopy(renderPhoto,SDL_CreateTextureFromSurface(renderPhoto,text),NULL,&inf);
            textInPhoto = TTF_RenderText_Solid(robotoInfo,photoInfo->nombre,color);
            SDL_RenderCopy(renderPhoto,SDL_CreateTextureFromSurface(renderPhoto,textInPhoto),NULL,&inf2);        
            SDL_FreeSurface(text);
            SDL_FreeSurface(textInPhoto);
            text = TTF_RenderText_Solid(robotoInfo,"Size",color);
            inf.y = inf2.y = 200 + 490;
            SDL_RenderCopy(renderPhoto,SDL_CreateTextureFromSurface(renderPhoto,text),NULL,&inf);
            textInPhoto = TTF_RenderText_Solid(robotoInfo,photoInfo->nombre,color);
            SDL_RenderCopy(renderPhoto,SDL_CreateTextureFromSurface(renderPhoto,textInPhoto),NULL,&inf2);
            SDL_RenderPresent(renderPhoto);
        }
        
        SDL_Log("Rect: x=%d y=%d w=%d h=%d\n",fixedSize.x,fixedSize.y,fixedSize.w,fixedSize.h);
        SDL_RenderCopy(renderPhoto,shownPhoto,NULL,&fixedSize);
        SDL_RenderPresent(renderPhoto);
    }
    SDL_DestroyWindow(photo);
    SDL_DestroyTexture(shownPhoto);
    SDL_DestroyRenderer(renderPhoto);
    imageShown = false;
}
