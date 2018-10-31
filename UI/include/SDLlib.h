#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

int SDL_STARTER(SDL_Window **window,SDL_Surface **windowSurface, char message[100]);
void SDL_PRINT(SDL_Surface **imageSurface, SDL_Surface **windowSurface, SDL_Window **window);
void IMG_Loader(SDL_Surface **image, char PATH[100]);



#define center  SDL_WINDOWPOS_CENTERED
#define HEIGHT 600
#define WEIGHT 600

int SDL_STARTER(SDL_Window **window,SDL_Surface **windowSurface, char message[100]){
    /**
     * Initialize SDL, creates a window and asign a window surface
    */
    *window = SDL_CreateWindow(message,center,center,WEIGHT,HEIGHT,SDL_WINDOW_SHOWN);
    *windowSurface = SDL_GetWindowSurface(*window);//create a new window to renderer
    if(!*window){
        fprintf(stderr,"[ERR]ERROR CREATING WINDOW\n");
        return -1;
    }else{
        return 0;
    }
}

void SDL_PRINT(SDL_Surface **imageSurface, SDL_Surface **windowSurface, SDL_Window **window){
    /** 
     * Prints an image in the surface and reloads the window
    */
    SDL_BlitSurface(*imageSurface,NULL,*windowSurface,NULL);//print image in the new renderer
    SDL_UpdateWindowSurface(*window);//update the window
}

void IMG_Loader(SDL_Surface **image, char PATH[100]){
    printf("%s",PATH);
    *image = IMG_Load(PATH);//loads an image
}