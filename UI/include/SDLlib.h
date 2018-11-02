#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


#define center  SDL_WINDOWPOS_CENTERED
#define HEIGHT 450
#define WIDTH 450
#define pressedkey key.keysym.sym

int SDL_STARTER(SDL_Window **window,SDL_Renderer **renderer, char message[100]){
    /**
     * Initialize SDL, creates a window and asign a window surface
    */
    *window = SDL_CreateWindow(message,center,center,WIDTH,HEIGHT,SDL_WINDOW_SHOWN);
    *renderer = SDL_CreateRenderer(*window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    SDL_StartTextInput();
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

SDL_Surface *OptimizedSurface(char filepath[100],SDL_Surface *windowSurface){
    SDL_Surface *optimizedSurface = NULL;
    SDL_Surface *surface = NULL;
    IMG_Loader(&surface,filepath);  
    if(!surface){
        printf("ERROR");
    }else{
        optimizedSurface = SDL_ConvertSurface(surface,windowSurface->format,0);
    }
    SDL_FreeSurface(surface);
    return optimizedSurface;
}

SDL_Texture *LoadTexture(char filePath[100],SDL_Renderer *renderTarget){
    SDL_Texture *texture = NULL;
    SDL_Surface *surface = NULL;
    IMG_Loader(&surface,filePath);
    if(!surface){
        printf("ERROR");
    }
    texture = SDL_CreateTextureFromSurface(renderTarget,surface);
    if(!texture){
        printf("ERROR TEXTURE, %s\n",SDL_GetError());
    }   
    SDL_FreeSurface(surface);
    return texture;
}