// #include <stdio.h>
// #include <SDL2/SDL.h>
// #include <SDL2/SDL_image.h>
// #include <stdbool.h>
// #include <math.h>
// #include <SDL2/SDL_ttf.h>
// #include "SDLlib.h"
// #include "UIChargingScreen.h"

// extern bool loading;

// int ChargingScreen(){
//     printf("Entre");
//     //SDL_Init(SDL_INIT_VIDEO);
//     bool forced = false;;
//     int xAux,yAux,cont=0,cont2=0,ff=1,MaxCont=0,aux,MAX = 8;
//     Tiempo Time;
//     Color color;
//     color.a=255;
//     //SDL_VARIABLES
//     SDL_Window *window = NULL;//principal window
//     SDL_Event event;//the event checker
//     SDL_Renderer *main_render;
//     SDL_Texture *logo = NULL;
//     SDL_Texture *background = NULL;
//     //WE will use renderer for better results
//     if(SDL_STARTER(&window,&main_render,"cimage")==0){
//         // printf("Image created");
//         logo = LoadTexture("./resources/CIMAGE33.png",main_render);
//         background = LoadTexture("./resources/background.jpg",main_render);
//         Time.frameTime = Time.deltaTime = 0;
//         Time.currentTime = Time.prevTime = 0;
//         //game cycle
//         while(loading){
//             SDL_TIME(&Time.prevTime,&Time.currentTime,&Time.deltaTime);
//                 while(SDL_PollEvent(&event)!=0){//EVENTS
//                 if(event.type == SDL_QUIT){// || (event.type == SDL_KEYDOWN && event.pressedkey==SDLK_ESCAPE)){
//                     loading=false;
//                     forced = true;
//                 }
//             }
        
//             // Render Printing
            
//             SDL_RenderClear(main_render);
//             // printf("I cleared\n");
//             SDL_RenderCopy(main_render,background,NULL,NULL);
//             // printf("I backgrounded\n");
//             SDL_RenderCopy(main_render,logo,NULL,NULL);
//             // printf("I printed CIMAGE\n");
//             for(int i=cont2; i<cont; i++){//charging rullete
//                 aux = (360/MAX);
//                 xAux = 20 * cos((i*aux)*RAD);
//                 yAux = -20 * sin((i*aux)*RAD);
//                 SDL_SetRenderDrawColor(main_render,255,255,255,255);
//                 SDL_FilledCircle(main_render,4,HEIGHT/2 + xAux, WIDTH/2 + 160 + yAux);
//                 // printf("I created circle %d\n",i);
//             }

//             SDL_RenderPresent(main_render);
//             // printf("I Presented\n");
//             Time.frameTime += Time.deltaTime;
//             // printf("%.2f\n",Time.frameTime);
//             if(Time.frameTime>=.5){
//                 if(cont<MAX){
//                     cont++;
//                 }else{
//                     if(cont2<MAX){
//                         cont2++;
//                     }else{
//                         cont2=0;
//                         cont=0;
//                     }
//                 }
//                     Time.frameTime = 0;
//                     MaxCont++;
//             }            
//         }
//         SDL_DestroyWindow(window);
//         SDL_DestroyRenderer(main_render);
//         //SDL_Quit();
        
//     } 

// }
