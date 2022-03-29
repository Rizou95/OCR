#include <math.h>
#include <string.h>
#include <err.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_rotozoom.h"
#include "../utils/sdl_utils.h"

int max(int array[], int size){
    int tmp = array[0];
    int index = 0;
    for (int x = 1; x < size - 1; x++){
        if (array[x] >= tmp){
            tmp = array[x];
            index = x;
        }
    }
    return index;
}

void BFS(SDL_Surface *imgs, int *wmax)
{
    int nbpixels = 0;
    int arrayx[(imgs->w)];
    for (int x = 0; x < imgs->w; x++){
        for (int y = 0; y < imgs->h; y++){
            Uint8 r,g,b;
            Uint32 pixel = get_pixel(imgs, x , y);
            SDL_GetRGB(pixel, imgs->format, &(r), &(g), &(b));
            if((r<200)&(g<200)&(b<200)){
                nbpixels += 1;
            } 
        } 
        arrayx[x] = nbpixels;
        nbpixels = 0;
    }
    for (int a = 0; a < 10; a++){
        int maxx = max(arrayx, imgs->w);
        wmax[a] = maxx;
        for (int g = -5; g < 5; g++){
            if (maxx + g > 0 && maxx + g < imgs->w - 1){
                arrayx[maxx + g] = 0;
            }
        }
    }
}

void DFS(SDL_Surface *imgs, int *hmax){
    int nbpixels = 0;
    int arrayy[(imgs->h)];
    for (int i = 0; i < imgs->h; i++){
        for (int j = 0; j < imgs->w; j++){
            Uint8 r,g,b;
            Uint32 pixel = get_pixel(imgs, j , i);
            SDL_GetRGB(pixel, imgs->format, &(r), &(g), &(b));
            if((r<200)&(g<200)&(b<200)){
                nbpixels += 1;
            } 
        }
        arrayy[i] = nbpixels;
        nbpixels = 0; 
    }
    for (int a = 0; a < 10; a++){
        int maxx = max(arrayy, imgs->h);
        hmax[a] = maxx;
        for (int g = -5; g < 5; g++){
            if (maxx + g > 0 && maxx + g < imgs->h - 1){
                arrayy[maxx + g] = 0;
            }
        }
    }
}

int compare_function(const void *a,const void *b) 
{
    int *x = (int *) a;
    int *y = (int *) b;
    return *x - *y;
}

void coloration(SDL_Surface* imgs){
    int *wmax = malloc(10 * sizeof(int));
    int *hmax = malloc(10 * sizeof(int));
    BFS(imgs, wmax);
    DFS(imgs, hmax);
    /*Uint32 pixel = SDL_MapRGB(imgs->format, 255, 0, 0);
    for (unsigned i = 0; i < 10; i++){
        for (unsigned j = 0; j < (unsigned)imgs->h; j++){
            put_pixel(imgs, wmax[i], j, pixel);
        }
    }
    for (unsigned k = 0; k < 10; k++){
        for (unsigned l = 0; l < (unsigned)imgs->w; l++){
            put_pixel(imgs, l, hmax[k], pixel);
        }
    }*/
    //Saving each box as an image
    SDL_Rect position;
    SDL_Surface *cell;
    qsort(hmax,10,sizeof(int),compare_function);
    qsort(wmax,10,sizeof(int),compare_function);
    int x = 1;
    for (long unsigned int j = 0; j <= sizeof(hmax); j++)
    {
        for (long unsigned int i = 0; i <= sizeof(wmax); i++)
        {
            cell = SDL_CreateRGBSurface(SDL_HWSURFACE, wmax[i+1] - wmax[i] - 2,
             hmax[j+1] - hmax[j] - 2, 32, 0, 0, 0, 0);
            position.x = wmax[i];
            position.y = hmax[j];
            position.w = hmax[j+1] - hmax[j];
            position.h = wmax[i+1] - wmax[i];
            SDL_BlitSurface(imgs,&position,cell,NULL);
            char buffer[20];

            double ratioX = ((double)28)/((double)cell->w);
            double ratioY = ((double)28)/((double)cell->h);

            cell = rotozoomSurfaceXY(cell, 0, ratioX, ratioY, 1);
            snprintf(buffer, sizeof(buffer), "./case/%d.bmp", x);
            SDL_SaveBMP(cell, buffer);

            SDL_FreeSurface(cell);
            x++;        
        }
    }
    free(hmax);
    free(wmax);
}