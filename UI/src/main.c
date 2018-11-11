#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_audio.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h>
#include "detect-plataform.h"
//#include "system.h"
#include "list.h"
//FIXME: Im not stable, you should use bitmap font
#include <SDL2/SDL_ttf.h>
#include "SDLlib.h"

#define dim 100
#define pressedkey key.keysym.sym
#define ResourcePath "./resources/"

#define PI 3.1416
#define RAD PI/180

typedef struct{
    SDL_Rect rectImage;
    SDL_Texture *textureImage;
    int index;
    int date;
}Image;


typedef struct menuData{
    char dataTitle[100];
    DoubleList *option;
}menuData;

typedef struct{
    int x;
    int xF;
    int y;
    int yF;
}menuPos;

#ifdef LINUX
    void cleanScreen();
#elif PLATAFORM_WINDOWS
    void cls();
#endif

// TODO: are we cleanning up the console and the screen??
// do we want that to happen?
// void renderScreen (SDL_Renderer *renderer, SDL_Texture *screen, UIScreen *scene) {

//     // render the views from back to front for the current screen
//     UIView *v = NULL;
//     for (ListElement *e = LIST_START (scene->views); e != NULL; e = e->next) {
//         v = (UIView *) LIST_DATA (e);
//         clearConsole (v->console);
//         v->render (v->console);
//         SDL_UpdateTexture (screen, v->pixelRect, v->console->pixels, v->pixelRect->w * sizeof (u32));
//     }

//     SDL_RenderClear (renderer);
//     SDL_RenderCopy (renderer, screen, NULL, NULL);
//     SDL_RenderPresent (renderer);

// }

uint32_t ChargingScreen(){
    // printf("Entre");
    SDL_Init(SDL_INIT_VIDEO);
    bool eeeeeeeexit = false, forced = false;;
    int xAux,yAux,cont=0,cont2=0,ff=1,MaxCont=0,aux,MAX = 8;
    Tiempo Time;
    Color color;
    color.a=255;
    //SDL_VARIABLES
    SDL_Window *window = NULL;//principal window
    SDL_Event event;//the event checker
    SDL_Renderer *main_render;
    SDL_Texture *logo = NULL;
    SDL_Texture *background = NULL;
    //WE will use renderer for better results
    if(SDL_STARTER(&window,&main_render,"cimage")==0){
        // printf("Image created");
        logo = LoadTexture(ResourcePath"/CIMAGE33.png",main_render);
        background = LoadTexture(ResourcePath"/background.jpg",main_render);
        Time.frameTime = Time.deltaTime = 0;
        Time.currentTime = Time.prevTime = 0;
        //game cycle
        while(!eeeeeeeexit){
            SDL_TIME(&Time.prevTime,&Time.currentTime,&Time.deltaTime);
                while(SDL_PollEvent(&event)!=0){//EVENTS
                if(event.type == SDL_QUIT){// || (event.type == SDL_KEYDOWN && event.pressedkey==SDLK_ESCAPE)){
                    eeeeeeeexit=true;
                    forced = true;
                }
            }
            /*
            *Render Printing
            * */
            SDL_RenderClear(main_render);
            // printf("I cleared\n");
            SDL_RenderCopy(main_render,background,NULL,NULL);
            // printf("I backgrounded\n");
            SDL_RenderCopy(main_render,logo,NULL,NULL);
            // printf("I printed CIMAGE\n");
            for(int i=cont2; i<cont; i++){//charging rullete
                aux = (360/MAX);
                xAux = 20 * cos((i*aux)*RAD);
                yAux = -20 * sin((i*aux)*RAD);
                if(i%4==0){
                    //SDL_RandomColor(main_render,color);
                }
                SDL_SetRenderDrawColor(main_render,255,255,255,255);
                SDL_FilledCircle(main_render,4,HEIGHT/2 + xAux, WIDTH/2 + 160 + yAux);
                // printf("I created circle %d\n",i);
            }
            SDL_RenderPresent(main_render);
            // printf("I Presented\n");
            Time.frameTime += Time.deltaTime;
            // printf("%.2f\n",Time.frameTime);
            if(Time.frameTime>=.5){
                if(cont<MAX){
                    cont++;
                }else{
                    if(cont2<MAX){
                        cont2++;
                    }else{
                        cont2=0;
                        cont=0;
                    }
                    
                }
                    Time.frameTime = 0;
                    MaxCont++;
            }
            if(MaxCont==2*MAX){
                eeeeeeeexit=true;
            }
            
        }
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(main_render);
        SDL_Quit();
        if(forced){
            return -1;
        }else{
            return 0;
        }
    }
}
//FIXME: Im not efficient
//TODO: How Can This function be more eficcient
//TODO: OPEN-GL?????
SDL_Texture * SDL_menu(SDL_Renderer **main_render,DoubleList **menu, TTF_Font *Sans, SDL_Color Black,Color color, menuPos pos[5], bool selected, int numSelected){
    int i;
    if(!*menu){
        *menu = dlist_init(free);
        menuData *data;
        menuData option;
        char messages[6][100]={"File\0","Edit\0","Insert\0","View\0","Help\0",'\0'};
        char Opt[5][20][20]={{"New","Open","Save","Save As", "Import", "Export", "Exit",'\0'},
                            {"Copy","Cut","Paste",'\0'},
                            {"//NOT YET","//NOT YET","//NOT YET","//NOT YET",'\0'},
                            {"FullScreen","View As frames", "List",'\0'},
                            {"Ask","See manual",'\0'}};
        for( i=0; messages[i][0]!='\0'; i++){
            data = (menuData * ) malloc (sizeof(menuData));
            strcpy(data->dataTitle,messages[i]);
            data->option = dlist_init(free);
                    for(int j = 0;  Opt[i][j][0]!='\0'; j++){
                        strcpy(option.dataTitle , Opt[i][j]);
                        option.option = NULL;
                        
                        if(data->option->start){
                            dlist_insert_after(data->option,LIST_END(data->option),&option);
                        }else{
                            dlist_insert_after(data->option,LIST_START(data->option),&option);
                        }
                    } 
            if((*menu)->start){                
                dlist_insert_after(*menu, LIST_END(*menu),data);
            }else{
                dlist_insert_after(*menu,LIST_START(*menu),data);
            }
        }
    }
    SDL_Rect display, select[5];
    SDL_Surface *text; 
    SDL_Texture *textureTarget = NULL;
    Color rectColor,blackColor,blue,white;
    //97, 193, 215
    blue.a=140;
    blue.r=97;
    blue.g=193;  
    blue.b=215;
    white.a  = white.r = white.g = white.b = 255;
    rectColor.r=rectColor.g=rectColor.b=100;
    blackColor.r=blackColor.g=blackColor.b=70;
    rectColor.a=blackColor.a=200;
    SDL_Texture *texture;
    SDL_Color Gray = {blackColor.r,blackColor.g,blackColor.b};
    SDL_Color White = {255,255,255};
    char aux[20];
    ListElement *e = LIST_START(*menu);
    display.w = 0;
    //FIXME:Maybe here is the problem
    SDL_SetRenderTarget(*main_render,textureTarget);
    for(i=0; i<5; i++){
        menuData *h = (menuData *) e->data;
        strcpy(aux , h->dataTitle);
        display.x=55 * i;
        display.y=0;
        display.h=20;
        display.w=8*strlen(aux);
        select[i].x = display.x-10;
        select[i].y = display.y;
        select[i].w = display.w+20;
        select[i].h = display.h;
        if(pos[0].x=='\0'){
            pos[i].x=display.x;
            pos[i].xF=display.x+display.w;
            pos[i].y=display.y;
            pos[i].yF=display.h;
        }
        if(i==numSelected && selected){
            SDL_RenderFillRect(*main_render,&select[numSelected]);
            //text = TTF_RenderText_Solid(Sans,aux,White);//FIXME: Sometimes I give seg fault?
        }else{
            //text = TTF_RenderText_Solid(Sans,aux,Gray);//FIXME: Sometimes I give seg fault?
        }
        //texture = SDL_CreateTextureFromSurface(*main_render,text);    
        //SDL_FreeSurface(text);
    
        //SDL_RenderCopy(*main_render,texture,NULL, &display);
        //SDL_DestroyTexture(texture);
        e=e->next;
    }
    SDL_SetRenderTarget(*main_render,NULL);
    return textureTarget;
}


void randomDate(Image *imagenes){
    int w = 153, h = 80, x=0, y = 0;
    for(int i=0; i<50; i++){
        if(i<12){
            imagenes[i].date = 1;
        }else if(i<24){
            imagenes[i].date = 2;
        }else if(i<37){
            imagenes[i].date = 3;
        }else{
            imagenes[i].date = 4;
        }
        imagenes[i].index = i;
        imagenes[i].rectImage.x=30 + (w+30)*x;
        imagenes[i].rectImage.y=147 + (h+20)*y;
        imagenes[i].rectImage.h=h;
        imagenes[i].rectImage.w=w;
        x++;
        if(x >= 6){
            x=0;
            y++;
        }
        if(i>100){
            break;
        }
    }
}

SDL_Texture *ImagePrinting (SDL_Renderer **main_render, Image *imagenes) {

    if (*(main_render) && imagenes) {
        SDL_Rect camera;
        SDL_Texture *cuadrito = LoadTexture ("./resources/rectp.png", *main_render);

        if (cuadrito) {
            SDL_Texture *main_texture = SDL_CreateTexture (*main_render,
                SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 1270, 600);
            SDL_SetTextureBlendMode(main_texture,SDL_BLENDMODE_BLEND);
            SDL_SetRenderTarget (*main_render, main_texture);

            for (int i = 0; i < 2; i++) 
                if(imagenes[i].rectImage.y > 140 && imagenes[i].rectImage.y < 720)
                    SDL_RenderCopy (*main_render,cuadrito, NULL, &imagenes[i].rectImage);

            SDL_SetRenderTarget(*main_render,NULL);
            return main_texture;
        }

        else return NULL;
    }

    else return NULL;
    
}

SDL_Texture *texture_loadImages (SDL_Renderer **main_render, Image *imagenes) {

    if (*(main_render) && imagenes) {
        SDL_Rect camera;
        SDL_Texture *cuadrito = LoadTexture ("./resources/rectp.png", *main_render);

        if (cuadrito) {
            SDL_Texture *main_texture = SDL_CreateTexture (*main_render,
                SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 1270, 600);
            SDL_SetTextureBlendMode(main_texture,SDL_BLENDMODE_BLEND);
            SDL_SetRenderTarget (*main_render, main_texture);

            for (int i = 0; i < 2; i++) 
                if(imagenes[i].rectImage.y > 140 && imagenes[i].rectImage.y < 720)
                    SDL_RenderCopy (*main_render,cuadrito, NULL, &imagenes[i].rectImage);

            SDL_SetRenderTarget (*main_render,NULL);
            return main_texture;
        }

        else {
            printf ("hola");
            return NULL;
        } 
    }

    else {
        printf ("hola");
        return NULL;
    } 

}

SDL_Texture *texture_updateTexture (SDL_Renderer **main_render, Image *imagenes) {

    // checar que la textura tenga imagenes que imprimir
        // se movieron las posiciones de las imagenes con el scroll

    imagenes[1].rectImage.y += 7;
    imagenes[1].rectImage.y -= 7;

}

void User(){
//SDL Variables
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = NULL;
    SDL_Renderer *main_render = NULL, *imageRenderer = NULL;
    SDL_Event event;
    SDL_Surface *surface = NULL;
    SDL_Texture *texture[100], *menuTexture=NULL, *main_texture = NULL;
    SDL_Color White = {255,255,255}, Black = {0,0,0};
    SDL_Rect clk,scrollbar,scrollbarPos,date,upBar,downBar;
    SDL_Texture *infoPhoto[100];
    clk.x = 0;
    clk.y = 40;
    clk.h = 20;
    clk.w = 20; 
    date.x = date.y = 0;
    date.w  = 1280;
    date.h  = 20;
    scrollbar.x = 1270;
    scrollbar.y  = 125;
    scrollbar.h = 720-120;
    scrollbar.w = 10;
    scrollbarPos.x = 1270;
    scrollbarPos.y = 125;
    scrollbarPos.h = 30;
    scrollbarPos.w  = 10;
    upBar.x = upBar.y = 0;
    upBar.w = 1280;
    upBar.h = 125;
//TTF Variables
    // TTF_Init();
    DoubleList *menu=NULL;
    menuPos pos[5];

    memset (&pos, 0, sizeof (menuPos));

    //  TTF_Font *Roboto = TTF_OpenFont(ResourcePath"/Fonts/Roboto-Light.ttf",210);
//External variables
    Image *imagenes;
    Tiempo Time;
    Time.frameTime = 0;
    Color color,colorLine,grayScroll,darkGrayScroll;
    grayScroll.r = grayScroll.g = grayScroll.b = 110;
    darkGrayScroll.r = darkGrayScroll.g = darkGrayScroll.b = 140;
    color.a = colorLine.a = grayScroll.a = darkGrayScroll.a =  255;
    char times[100];
    menuPos auxiliar;
    // time_t rawTime;
    // struct tm * timeInfo;
    // time (&rawTime);
    // timeInfo = localtime(&rawTime);
    int numSelected = -1, txCount = 0, LY = 0, NY = 0;
    char counter[100],timee[100];
    bool eeeeeeeexit=false, selected = false;
    bool in_menu=false,clicked = false, active_menu = false;
    bool FullScreen=false,scrollHang = false,scroll=false;

//Process
    if(!SDL_STARTER_FIXED(&window,&main_render,"CIMAGE",1280,720)){
        color.r=color.g=color.b=230;//Light GRAY
        colorLine.r = colorLine.g = colorLine.b = 120;//GRAY
        SDL_SetWindowResizable(window,SDL_WINDOW_RESIZABLE);
        texture[0]=LoadTexture(ResourcePath"/CIMAGE10.png",main_render);
        // imagenes = (Image *)  malloc (sizeof(Image) * 100);
        imagenes = (Image *) calloc (100, sizeof (Image));
        randomDate(imagenes);
        Time.frameTime = 0;

        // TODO: this have to be dynamic --> move inside main loop
        // load images to memory
        main_texture = ImagePrinting (&main_render, imagenes);
        // main_texture = texture_loadImages (&main_render,imagenes);

        // FIXME: mandarlo a una perra propia funcion!!!!!!!! ----->>>>> mainLoop ();

        while(!eeeeeeeexit){//APP cycle
            SDL_TIME (&Time.prevTime,&Time.currentTime, &Time.deltaTime);//TIME FUNCTION
            // SDL_itoa(Time.frameTime,times,10);
            //  texture[1]=SDL_CreateTextureFromSurface(main_render,TTF_RenderText_Solid(Roboto,times,Black));

            /*** EVENTS ***/
            // FIXME: mandarlo a una perra propia funcion!!!!!!!! ----->>>>> handleEvents ();
            while(SDL_PollEvent(&event)!=0) {
                if(event.type == SDL_KEYDOWN || event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN){
                    Time.frameTime=0;
                }
                if(event.type==SDL_QUIT || (event.type ==SDL_KEYDOWN && event.pressedkey==SDLK_ESCAPE)){
                    eeeeeeeexit = true;
                }else if(event.type == click){
                    if(event.button.button == leftClick){
                        auxiliar.x = mouseX;
                        auxiliar.y = mouseY;
                    }
                    if(mouseY<=20 && !clicked){
                        for(int i=0; i<5; i++){
                            if(auxiliar.x>=pos[i].x && auxiliar.x<=pos[i].xF){
                                if(auxiliar.y>=pos[i].y && auxiliar.y<=pos[i].yF){
                                    clicked = true;
                                    selected  =true;
                                    numSelected = i;
                                    break;
                                }
                                else clicked = false;
                            }
                        }
                    }else{
                        clicked = false;
                        selected = false;
                        numSelected = -1;
                    }
                }else if(event.type == SDL_KEYDOWN && (event.pressedkey == SDLK_LALT || event.pressedkey == SDLK_RALT)){
                    active_menu = !active_menu;
                }else if(event.type == SDL_KEYDOWN && event.pressedkey == SDLK_F11){
                    SDL_fullScreenToggle(window,&FullScreen);
                }
                
                // OLD
                else if(event.type == SDL_MOUSEWHEEL){
                    if(event.wheel.y < 0 && scrollbarPos.y <= 695){
                        scrollbarPos.y += 7;
                        imagenes[1].rectImage.y += 7;
                        scroll = true;
                    }else if(event.wheel.y > 0 && scrollbarPos.y >= 130){
                        scrollbarPos.y -= 7;
                        imagenes[1].rectImage.y -= 7;
                        scroll = true;
                    }
                }

                // NEW -> SDL_Texture *texture_updateTexture ()
                // else if(event.type == SDL_MOUSEWHEEL) scroll = true;
                
                else if(event.type == SDL_MOUSEBUTTONUP && scrollHang){
                    scrollHang = false;
                    SDL_GetMouseState(NULL,&NY);
                    printf("realeased click; LY= %d, NY = %d\n",LY,NY);

                }
                if(event.type == click){
                    if(event.button.button == leftClick  && mouseX >= 1270 && mouseY >= 125){
                        scrollHang = true;
                        LY = mouseY;
                    } 
                }if(scrollHang){
                    SDL_GetMouseState(NULL,&scrollbarPos.y);
                    printf("New pos: %d\n",scrollbarPos.y - LY);
                    imagenes[1].rectImage.y = imagenes[0].rectImage.y - (LY-scrollbarPos.y);
                    scroll = true;
                }
            }

            /*** RENDERER ***/
            
            SDL_RenderClear (main_render);
            //UpBar
            SDL_RenderColor(&main_render,color);



            if (scroll) {
                // printf("Changes were made");
                // main_texture = texture_updateTexture (&main_render,imagenes);
                // if(!main_texture){
                //     // printf("Cant Charge images-- %s\n",SDL_GetError());
                // }

                // TODO:
                // SDL_UpdateTexture (screen, v->pixelRect, v->console->pixels, v->pixelRect->w * sizeof (u32));

                // SDL_UpdateTexture (main_texture, image_rect, )
            }



            scroll = false;
            SDL_RenderCopy(main_render,main_texture,NULL,NULL);
            SDL_SetRenderDrawColor(main_render,color.r,color.g,color.b,color.a);
            SDL_RenderFillRect(main_render,&upBar);
            SDL_Print(&main_render,texture[0],1280-138,0,128,128);
            SDL_RenderColor(&main_render,colorLine);
            SDL_RenderDrawLine_Gross(&main_render,0,1280,125,125,2);
            // if(active_menu){
            //     menuTexture=SDL_menu(&main_render,&menu,Roboto,Black,color,pos,selected,numSelected);
            // }
            SDL_RenderCopy(main_render,menuTexture,NULL,NULL);
            SDL_RenderColor(&main_render,grayScroll);
            //ScrollBar
            SDL_RenderFillRect(main_render,&scrollbar);
            SDL_RenderColor(&main_render,darkGrayScroll);
            SDL_RenderFillRect(main_render,&scrollbarPos);
            //Images
            SDL_RenderColor(&main_render,color);
            //TODO: present images
            //TODO: Scroll bar should move the y value of all the rects
            //Clock
            SDL_RenderCopy(main_render,texture[1],NULL,&clk);
            SDL_RenderPresent(main_render);
///////////////////////////////////////////////////////////////////////////////////////////////////            
            Time.frameTime += Time.deltaTime;
        
            if(Time.frameTime==5){
                printf("Entre");
                system("pkill UIexec");
                eeeeeeeexit=true;
            }
        }
        SDL_Texture *tex=NULL;
        /*tex = texture[txCount];
        while(tex){
            txCount++;
            tex=texture[txCount];
        }
        txCount--;
        for(int i=0; i<txCount; i++){
            SDL_DestroyTexture(texture[i]);
        }*/
        clean(&window,&main_render);
    
    }
}

int main(void){
    uint32_t cont=0;
    printf("Start");

    // if (!ChargingScreen()) User ();
    // else return 1;

    User ();

    return 0;


} 