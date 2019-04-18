#define _DEFAULT_SOURCE
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
#include "photo.h"

#pragma region OTHER

#include <stdarg.h>

char *createString (const char *stringWithFormat, ...) {

    char *fmt;

    if (stringWithFormat != NULL) fmt = strdup (stringWithFormat);
    else fmt = strdup ("");

    va_list argp;
    va_start (argp, stringWithFormat);
    char oneChar[1];
    int len = vsnprintf (oneChar, 1, fmt, argp);
    if (len < 1) return NULL;
    va_end (argp);

    char *str = (char *) calloc (len + 1, sizeof (char));
    if (!str) return NULL;

    va_start (argp, stringWithFormat);
    vsnprintf (str, len + 1, fmt, argp);
    va_end (argp);

    free (fmt);

    return str;

}

#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

// DoubleList *fileNames = NULL;
char **fileNames = NULL;

void file_openAllFilesInDir (char *directory) {

    if (directory) {
        DIR *FD;
        struct dirent *in_file;
        FILE *common_file;

        if (NULL == (FD = opendir (directory))){
            fprintf(stderr, "Error : Failed to open input directory - %s\n", strerror(errno));
            fclose(common_file);

            return;
        }

        // fileNames = dlist_init (free);
        fileNames = (char **) calloc (30, sizeof (char *));
        int count = 0;

        while ((in_file = readdir(FD))) {
            /* On linux/Unix we don't want current and parent directories
            * On windows machine too, thanks Greg Hewgill
            */
            if (!strcmp (in_file->d_name, "."))
                continue;
            if (!strcmp (in_file->d_name, ".."))    
                continue;
            /* Open directory entry file for common operation */
            /* TODO : change permissions to meet your need! */
            // entry_file = fopen(in_file->d_name, "rw");
            // if (entry_file == NULL)
            // {
            //     fprintf(stderr, "Error : Failed to open entry file - %s\n", strerror(errno));
            //     fclose(common_file);

            //     return 1;
            // }

            
            char *filename = createString ("./imgs/%s", in_file->d_name);
            fileNames[count] = filename;
            count++;
            // printf ("%s\n", in_file->d_name);
        }

        void set_total_img_count (unsigned int count);
        void set_render_img_count (unsigned int count);
        set_total_img_count (count);
        set_render_img_count (count);

    }

}


#pragma endregion

bool loading = false,imageShown = false;

// images being rendered to the secreen
unsigned int RENDER_IMG_COUNT = 0;

// total number of imgs that we expect to be on the list
unsigned int TOTAL_IMG_COUNT = 0;

void set_total_img_count (unsigned int count) { TOTAL_IMG_COUNT = count; }
void set_render_img_count (unsigned int count) { RENDER_IMG_COUNT = count; }

int imageCharger(Image *imagenes,SDL_Renderer *main_renderer){
    int x = 1, y = 1;

    for(int i=0; i < RENDER_IMG_COUNT; i++){
        imagenes[i].index = i;
        imagenes[i].ImageRect.x = (110 * x) - 60;
        imagenes[i].ImageRect.y = (140 * y);
        imagenes[i].ImageRect.w = 100;
        imagenes[i].ImageRect.h = 80;
        imagenes[i].Info;
        char *b = (char *) calloc (100, sizeof (char));
        // if(i<=20){
        //     // strcpy(imagenes[i].Path,"./resources/background.jpg");
        //     // strcpy(b,"I'm a red photo");
        // }else{
        //     // strcpy(imagenes[i].Path,"./resources/rectg.png");
        //     // strcpy(b,"I'm a green photo");
        // }

        strcpy(imagenes[i].Path, fileNames[i]);

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
        if(x==7){
            y++;
            x=1;
        }
    }
    loading = false;
    return y;
}

void ImageMover(Image *imagenes, int Ymove){
    for(int i=0; i < RENDER_IMG_COUNT; i++){
        imagenes[i].ImageRect.y += Ymove;
    }
}

int imageLoader(SDL_Renderer *main_renderer, Image *imagenes){
    if(!(imagenes->image)){
        imagenes->image = LoadTexture(imagenes->Path,main_renderer);
        // printf("Index Picture: %d --Texture Created\n",imagenes->index);
    }
    imagenes->isShown = true;
}


void imageCleaner(Image *imagenes){
    if(imagenes->image){
        SDL_DestroyTexture(imagenes->image);
        // printf("Index Picture: %d --Texture Destroyed\n",imagenes->index);
        imagenes->image=NULL;
    }
    imagenes->isShown = false;
}
//TODO: To make an efficient way to work, We use threads, to charge and render gray rects while
//the real photo is charging
void imagePrinting(Image *imagenes, SDL_Renderer **main_renderer, SDL_Texture **imageTexture, SDL_Thread *ch_t){
    SDL_SetRenderTarget(*main_renderer,*imageTexture);
    for(int i=0; i < RENDER_IMG_COUNT; i++){
        char *b = (char *)imagenes[i].Info;
        //printf("Info photo with index:%d -- %s\n",imagenes[i].index,b);
        if(imagenes[i].ImageRect.y>80 && imagenes[i].ImageRect.y<745){
            //This line should enter just once per cycle and only if new information is charged
            if(imagenes[i].isShown==false){
                imageLoader(*main_renderer,&imagenes[i]);
            }
            SDL_RenderCopy(*main_renderer,imagenes[i].image,NULL,&imagenes[i].ImageRect);
            //TODO:I dont Access to this one if a thread in this line is running
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

int photoSelector(void *data){
    photoSelect_data *info = (photoSelect_data *)data;
    Image *imagen = info->imagenes;
    int i,x,y;
    x = info->x;
    y = info->y;
    while(imagen[i].isShown==false){
        i++;
        // SDL_Log("%d\n",i);
    }
    while(imagen[i].isShown){
        if(x>=imagen[i].ImageRect.x && 
            x<=imagen[i].ImageRect.w + imagen[i].ImageRect.x &&
                y>=imagen[i].ImageRect.y && y<=imagen[i].ImageRect.y + imagen[i].ImageRect.h){
            imageShown = true;
            openPhoto(imagen[i]);
                SDL_Log("4Here'snot");

            break;
        }
        i++;
    }
}


// old ui entry point
void user(){

    file_openAllFilesInDir ("./imgs/");

    // FIXME: 
    // RENDER_IMG_COUNT = 10;

    TTF_Init();
    void *res;
    SDL_DisplayMode current;
    SDL_Init(SDL_INIT_VIDEO);
    Image *imagenes = (Image *)malloc(sizeof(Image)*RENDER_IMG_COUNT);
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
    scrollBarPos.h = 30;
    logoRect.x = 1280-90;
    logoRect.y = 10;
    logoRect.w = logoRect.h = 90;
    upBar.x = upBar.y = 0;
    upBar.w = SCREEN_WIDTH;
    upBar.h = 124;
    bool running = true,showMenu = false, Selected = false,scrollHang = false;
    bool fullScreen = false, menuInMenu[5];
    for(int i=0; i<5; i++){
        menuInMenu[i]=false;
    }
    int numSelected = -1, newY=0,s,numNumSelected = -1;
    char Path[1024];
    DoubleList *menu = NULL;
    TTF_Font *Roboto = TTF_OpenFont("./resources/Fonts/Roboto-Regular.ttf",210);
    SDL_Event e;
    menuPos pos[5];
    FILE *p = NULL;
    SDL_Thread *ChargeThread, *ch_t, *photo_t;
    if(!SDL_STARTER_FIXED(&window,&main_renderer,"CIMAGE",SCREEN_HEIGHT,SCREEN_WIDTH)){
        imageTexture= SDL_CreateTexture(main_renderer,SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING  ,SCREEN_WIDTH,SCREEN_WIDTH);

        menuTexture = SDL_CreateTexture(main_renderer,SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,1280,720);

        if(!imageTexture) printf("%s\n",SDL_GetError());

        SDL_Texture *logo = LoadTexture("./resources/CIMAGE10.png",main_renderer);

        scrollBarTexture = SDL_CreateTexture(main_renderer,SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,scrollBar.w,scrollBar.h);

        SDL_Texture *mainTexture = NULL;

        SDL_Color white = {255,255,255,255};

        loading = true;

        ChargeThread = SDL_CreateThread(ChargingScreen,"Carga de pantalla",NULL);
        int columns;
        columns = imageCharger(imagenes,main_renderer);

        if(!loading) SDL_DetachThread(ChargeThread);

        while(running){
            while(imageShown);//Pause if an image is showing
            SDL_TIME(&Time.prevTime,&Time.currentTime,&Time.deltaTime);
            
            while(SDL_PollEvent(&e)!=0){
                if(e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.pressedKey == SDLK_ESCAPE)){
                    running = false;
                }else if(e.type == SDL_KEYDOWN && (e.pressedKey == SDLK_LALT || 
                    e.pressedKey == SDLK_RALT)){
                    showMenu = !showMenu;
                }else if(e.type == SDL_MOUSEWHEEL){
                    if(e.wheel.y < 0 && scrollBarPos.y <=720-24){
                        //imagenes[0].h + 10 
                        //100% es columns *(imagenes[0].h + 10) //parte porcentual es 680
                        scrollBarPos.y += 1;
                        ImageMover(imagenes,-4.2);
                    }else if(e.wheel.y >0 && scrollBarPos.y >=126){
                        scrollBarPos.y -= 1;
                        ImageMover(imagenes,+4.2);
                    }
                }

                else if(e.type == SDL_MOUSEBUTTONUP && scrollHang){
                    scrollHang = false;
                }else if(e.type == SDL_MOUSEBUTTONDOWN  && e.button.button == leftClick && 
                    mouseX>1260 && mouseY>130){
                    scrollHang =true;

                }else if(e.type == SDL_KEYDOWN && e.pressedKey == SDLK_F11){
                    SDL_fullScreenToggle(window,&fullScreen);
                }else if(e.type == click && mouseY >125 && mouseY<720){
                    photoSelect_data *data=(photoSelect_data*)malloc(sizeof(photoSelect_data));
                    data->x = mouseX;
                    data->y = mouseY;
                    data->imagenes = imagenes;
                    photo_t = SDL_CreateThread(photoSelector,"Photo Selector",data);
                    SDL_Delay(10);
                }
                //TODO: I have to move scaled the scrollBar 
                
                if(scrollHang){
                    newY = scrollBarPos.y;
                    SDL_GetMouseState(NULL,&scrollBarPos.y);
                    newY = newY - scrollBarPos.y;
                    newY = newY * 4.2;
                    ImageMover(imagenes,newY);  
                }
                
                //MENU EVENTS
                if(showMenu){
                    if(e.type == click){
                        if(mouseY<=25){
                            for(int i=0; i<5; i++){
                                if(mouseX >= pos[i].x-10 && mouseX <= pos[i].xF+10){
                                    Selected = !Selected;
                                    numSelected = i;
                                    break;
                                }
                            }
                        }
                        if(mouseX>= pos[numSelected].x &&
                            mouseX<pos[numSelected].xF 
                            && mouseY>=25 && mouseY<=125){
                            if(numSelected==0){
                                if(mouseY>=25 && mouseY<=50){
                                    p = popen("zenity --file-selection --file-directory","r");
                                    fgets(Path,1024,p);
                                    pclose(p);
                                    Selected = !Selected;
                                    numSelected = -1;
                                }
                            }
                        }
                        if(mouseY>40 && Selected){
                            Selected = !Selected;
                            numSelected = -1;
                        }

                    }
                }
            }

            Time.frameTime += Time.deltaTime;
            if(Time.frameTime>=Time.deltaTime){
                SDL_RenderClear(main_renderer);
                SDL_SetRenderDrawColor(main_renderer,220,220,220,255);
                scrollBarPrint(scrollBar,scrollBarPos,&scrollBarTexture,main_renderer);
                imagePrinting(imagenes,&main_renderer,&imageTexture, ch_t);
                SDL_SetRenderDrawColor(main_renderer,100,100,100,255);
                SDL_RenderDrawLine_Gross(&main_renderer,0,1280,125,125,5);
                SDL_SetRenderDrawColor(main_renderer,220,220,220,255);
                SDL_RenderFillRect(main_renderer,&upBar);
                if(showMenu){
                    menuPrint(&main_renderer,Selected,numSelected,&menuTexture,Roboto,pos,&menu);
                }
                SDL_RenderCopy(main_renderer,logo,NULL,&logoRect);
                SDL_RenderPresent(main_renderer); 
                //SDL_Delay(5);  
                Time.frameTime = 0;
            }
        }
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(main_renderer);
        SDL_Quit();
        printf ("Done cleaning up!");
    }
}