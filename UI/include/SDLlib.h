#ifndef SDLLIB_H_
#define SDLLIB_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <time.h>
#include <stdbool.h>

#define pressedKey key.keysym.sym
#define  click SDL_MOUSEBUTTONDOWN
#define  leftClick SDL_BUTTON_LEFT
#define rightClick SDL_BUTTON_RIGHT 
#define mousePos SDL_MOUSEMOTION
#define mouseX e.button.x
#define mouseY e.button.y
#define center  SDL_WINDOWPOS_CENTERED
#define HEIGHT 450
#define WIDTH 450
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define pressedkey key.keysym.sym

typedef struct{
    int r,h,k,nX,nY,oX,oY;
    float step;
}Circle;

typedef struct{
    float frameTime;
    int prevTime;
    int currentTime;
    float deltaTime;
}Tiempo;

typedef struct{
    int r,g,b,a;
}Color;


extern int SDL_STARTER(SDL_Window **window,SDL_Renderer **renderer, char title[100]);
extern void clean(SDL_Window **window, SDL_Renderer **renderer);
extern int SDL_STARTER_FIXED(SDL_Window **window,SDL_Renderer **renderer, char title[100],int h,int w);
extern void IMG_Loader(SDL_Surface **image, char PATH[100]);
extern void SDL_exit(SDL_Event event, bool *exit);
extern SDL_Rect createRect(int x, int y);
extern SDL_Rect createRectFixed(int x,int y, int w,int h);
extern SDL_Texture *LoadTexture(char filePath[100],SDL_Renderer *renderTarget);
extern void SDL_TIME(int *prevTime,int *currentTime, float *deltaTime);
extern void SDL_Circle(SDL_Renderer *renderTarget, int r, int h, int k);
extern void SDL_FilledCircle(SDL_Renderer *renderTarget,int r,int h, int k);
extern void SDL_RandomColor(SDL_Renderer *renderTarget, Color color);
extern void SDL_RenderColor(SDL_Renderer **renderTarget, Color color);
extern void SDL_RenderDrawLine_Gross(SDL_Renderer **renderTarget, int x1, int x2, int y1, int y2, int thickness);
extern void SDL_fullScreenToggle(SDL_Window *window, bool *FullScreen);
extern SDL_DisplayMode currentWindow();


#endif