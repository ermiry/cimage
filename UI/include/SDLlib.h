#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <time.h>

typedef struct{
    int r,h,k,nX,nY,oX,oY;
    float step;
}Circle;

typedef struct{
    float frameTime;
    int prevTime ,currentTime;
    float deltaTime;
}Tiempo;

typedef struct{
    float frameWidth,frameHeight;
    int textureWidth,textureHeight;
}Sprite;

typedef struct{
    int r,g,b,a;
}Color;

#define center  SDL_WINDOWPOS_CENTERED
#define HEIGHT 450
#define WIDTH 450

#define pressedkey key.keysym.sym



int SDL_STARTER(SDL_Window **window,SDL_Renderer **renderer, char title[100]){
    /**
     * Initialize SDL, creates a window and asign a window surface
    */
    *window = SDL_CreateWindow(title,center,center,HEIGHT,WIDTH,SDL_WINDOW_SHOWN);
    *renderer = SDL_CreateRenderer(*window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC);

    if(!*window){
        fprintf(stderr,"[ERR]ERROR CREATING WINDOW\n");
        return -1;
    }else{
        return 0;
    }

}

int SDL_STARTER_FIXED(SDL_Window **window,SDL_Renderer **renderer, char title[100],int h,int w){
    *window = SDL_CreateWindow(title,center,center,h,w,SDL_WINDOW_SHOWN);
    *renderer = SDL_CreateRenderer(*window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC);
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

SDL_Texture *LoadTextureFixed(char filePath[100],SDL_Renderer *renderTarget, int sizeofSurface){
    
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

