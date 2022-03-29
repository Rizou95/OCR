#include <stdio.h>
#include <math.h>  
#include <string.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "SDL/SDL_rotozoom.h"
#include <err.h>
#include "detection.h"
#include "../utils/sdl_utils.h"

void getlines(SDL_Surface *image, int *x, int *y)
{
	int width = image->w;
	int height = image->h;
	int *linesx = malloc(height * sizeof(int));
	int *linesy = malloc(width * sizeof(int));
	int nbpixelx = 0;
	int nbpixely = 0;

	for(int y = 0; y < height ; y++) //nombre pixel sur hori
	{
		for(int x = 0; x < width ; x++)
		{
			Uint32 pixel = get_pixel(image,x,y);
			Uint8 r,g,b;
			SDL_GetRGB(pixel, image->format, &(r), &(g), &(b));
			if((r<150)&(g<150)&(b<150))
				nbpixelx += 1;
		}
		linesx[y] = nbpixelx;
		nbpixelx = 0;
	}

	for(int x = 0; x < width; x++) //verti
	{
		for(int y = 0; y < height; y++)
		{
			Uint32 pixel = get_pixel(image,x,y);
			Uint8 r,g,b;
			SDL_GetRGB(pixel, image->format, &(r), &(g), &(b));
			if((r<150)&(g<150)&(b<150))
				nbpixely += 1;
		}
		linesy[x] = nbpixely;
		nbpixely = 0;
	}
  
	for(int i = 0; i < 10;i++)
	{
		int ind = 0;
		int max = 0;
		for(int x = 0; x < height; x++)
		{
			if(linesx[x] > max)
			{
				max = linesx[x];
				ind = x;
			}
		}
		for(int j = ind - 8; j < ind + 8; j++)
		{
			if(j > -1 && j < height)
				linesx[j] = 0;
		}
		y[i] = ind;
	}


	for(int i = 0; i < 10;i++)
	{
		int ind = 0;
		int max = 0;
		for(int y = 0; y < width; y++)
		{
			if(linesy[y] > max)
			{
				max = linesy[y];
				ind = y;
			}
		}
		for(int j = ind - 8; j < ind + 8; j++)
		{
			if(j > - 1 && j < width)
				linesy[j] = 0;
		}
		x[i] = ind;
	}

    free(linesx);
    free(linesy);
}

float HoughTransform(SDL_Surface *image)
{
    int width = image->w;
    int height = image->h;
    
    int Rhos = sqrt(width * width + height * height);
    int Thetas = 180;

    int array[2*Rhos][Thetas];

    for (int i = 0; i < 2*Rhos; i++)
    {
        for (int j = 0; j < Thetas; j++)
            array[i][j] = 0;
    }

    Uint32 pixel;
    Uint8 r, g, b;
    int p = 0;
    double teta = 0;

    for (int x = 0; x < width; x += 2)
    {
        for (int y = 0; y < height; y += 2)
        {
            pixel = get_pixel(image, x, y);
            SDL_GetRGB(pixel, image->format, &(r), &(g), &(b));
            
            if (r+g+b < 450)
            {
                for (int t = 0; t < 180;t++)
                {
                    teta = ((double)t * M_PI) / 180;
                    p = x*cos(teta) + y*sin(teta) + Rhos;
                    array[p][t]++;
                }
            } 
            
        }
    }
    float ind = 0;
    int dalimit = 0;
    for(int i = 0; i < Thetas; i++)
    {
        for(int j = 0; j < 2 * Rhos; j++)
            if(array[j][i] > dalimit)
                {
                    dalimit = array[j][i];
                }
    }
    dalimit -= 10;
    int max = 0;
    
    int accu = 0;
    for(int i = 0; i < Thetas; i++)
    {
        for(int j = 0; j < 2 * Rhos; j++)
            if(array[j][i] > dalimit)
                accu += array[j][i];   
        if(accu > max)
        {
            max = accu;
            ind = i;
        }
        accu = 0;
    }
    return (ind == 0) ? 0 : ind - 90;
}



SDL_Surface* SDL_RotationCentralN(SDL_Surface* origine, float angle)
{
    SDL_Surface* destination;
    int i;
    int j;
    Uint32 color;
    int mx, my, mxdest, mydest;
    int bx, by;
    float angle_radian;
    float tcos;
    float tsin;
    double widthdest;
    double heightdest;
    
    angle_radian = -angle * M_PI / 180.0;
    
    tcos = cos(angle_radian);
    tsin = sin(angle_radian);
    
    widthdest=   ceil(origine->w * fabs(tcos) + origine->h * fabs(tsin)),
    heightdest=   ceil( origine->w * fabs(tsin) + origine->h * fabs(tcos)),
    
    
    destination = SDL_CreateRGBSurface(SDL_HWSURFACE, widthdest, heightdest, origine->format->BitsPerPixel,
                origine->format->Rmask, origine->format->Gmask, origine->format->Bmask, origine->format->Amask);
    
    if(destination==NULL)
    return NULL;
    
    mxdest = destination->w/2.;
    mydest = destination->h/2.;
    mx = origine->w/2.;
    my = origine->h/2.;
    
    for(j=0;j<destination->h;j++)
    for(i=0;i<destination->w;i++)
    {
    
    bx = (ceil (tcos * (i-mxdest) + tsin * (j-mydest) + mx));
    by = (ceil (-tsin * (i-mxdest) + tcos * (j-mydest) + my));
    if (bx>=0 && bx< origine->w && by>=0 && by< origine->h)
    {
        color = get_pixel(origine, bx, by);
        put_pixel(destination, i, j, color);
    }
    }
    SDL_SaveBMP(destination, "./tmp/rotate.bmp");
    return destination;
}

SDL_Surface* crop(SDL_Surface *image)
{
	int width = image->w;
    int height = image->h;
    SDL_Rect position;
    SDL_Surface *cell;
    Uint32 pixel;
    Uint8 r, g, b;
    int x = 0;
    int y = 0;
	for(; x < width; x++)
    {
        pixel = get_pixel(image, x, 1);
        SDL_GetRGB(pixel, image->format, &(r), &(g), &(b));
        if(r+g+b > 450)
            break;
    }
    if (x == width || x == 0)
        return NULL;

    for(; y < height; y++)
    {
        pixel = get_pixel(image, x, y + 1);
        SDL_GetRGB(pixel, image->format, &(r), &(g), &(b));
        if(r+g+b < 450)
            break;
    }
    if (y == height || y == 0)
        return NULL;

    y-=5;
    for(; x > 0; x--)
    {
        pixel = get_pixel(image, x, y);
        SDL_GetRGB(pixel, image->format, &(r), &(g), &(b));
        if(r+g+b == 0)
            break;
    }
    

    cell = SDL_CreateRGBSurface(SDL_HWSURFACE, width - (2*x), height - (2*y), 32, 0, 0, 0, 0);
    position.x = x;
    position.y = y;
    position.w = width -(2*x);
    position.h = height - (2*y);
    SDL_BlitSurface(image, &position, cell, NULL);
    SDL_SaveBMP(cell, "./tmp/crop.bmp");
    return cell;
}

SDL_Surface* colorationCrop(SDL_Surface *image)
{
    int *x = malloc(10 * sizeof(int));
	int *y = malloc(10 * sizeof(int));

	getlines(image,x,y);

	qsort(x,10,sizeof(int),compare_function);
	qsort(y,10,sizeof(int),compare_function);

    int a = 9, b = 0;
    int dimX = x[a] - x[b];
    int dimY = y[a] - y[b];

    while((dimX < dimY - 10 && dimX > dimY + 10) && dimX != dimY)
    {
        if (b < a)
            b++;
        else
        {
            b = 0;
            a--;
        }
    }
    dimX = x[a] - x[b];
    dimY = y[a] - y[b + 6];

    SDL_Rect position;
    SDL_Surface *cell;

    cell = SDL_CreateRGBSurface(SDL_HWSURFACE, dimX, dimY, 32, 0, 0, 0, 0);
    position.x = x[b];
    position.y = y[b+6];
    position.w = dimX;
    position.h = dimY;
    SDL_BlitSurface(image, &position, cell, NULL);
    free(x);
    free(y);
    SDL_SaveBMP(cell, "./tmp/crop2.bmp");
    return cell;
}

void lines(SDL_Surface *image)
{
    SDL_Surface *cell;
    SDL_Rect position;
    int width = image->w;
    int height = image->h;
    int wmax = width / 9;
    int hmax = height / 9;
    int x_coordinate[9];
    int y_coordinate[9];
    int x_index = 0;
    int y_index = 0;
    Uint32 pixel = SDL_MapRGB(image->format, 255, 0, 0);
    for(int x = 0; x < width; x += wmax)
    {
        for(int y = 0; y < height; y++)
            put_pixel(image, x, y, pixel);
        x_coordinate[x_index] = x;
        x_index++;
    }

    for(int y = 0; y < height; y += hmax)
    {
        for(int x = 0; x < width; x++)
            put_pixel(image, x, y, pixel);
        y_coordinate[y_index] = y;
        y_index++;
    }

    for(int y = 0; y < height; y++)
    {
        put_pixel(image, width -1, y, pixel);
    }

    for(int x = 0; x < width; x++)
    {
        put_pixel(image, x, height - 1, pixel);
    }

    int code = 1;
    for(int y = 0; y < 9; y++)
    {
        for(int x = 0; x < 9; x++)
        {
            cell = SDL_CreateRGBSurface(SDL_HWSURFACE, wmax, hmax, 32, 0, 0, 0, 0);
            position.x = x_coordinate[x];
            position.y = y_coordinate[y];
            position.w = wmax;
            position.h = hmax;
            SDL_BlitSurface(image,&position,cell,NULL);
            char buffer[20];
            
            double ratioX = ((double)28)/((double)cell->w);
            double ratioY = ((double)28)/((double)cell->h);

            cell = rotozoomSurfaceXY(cell, 0, ratioX, ratioY, 1);
            snprintf(buffer, 20, "./case/%d.bmp", code);
            SDL_SaveBMP(cell, buffer);
            
            code++;
            SDL_FreeSurface(cell);
        }
    }
}


/*
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("No image!\n");
        return 1;
    }
    SDL_Surface* image_surface;
    SDL_Surface* screen_surface;
    SDL_Surface* destination;
    SDL_Surface* croped;

    init_sdl(); 

    image_surface = load_image(argv[1]);
    screen_surface = display_image(image_surface);

    wait_for_keypressed();

    float teta = HoughTransform(image_surface);
    destination = SDL_RotationCentralN(image_surface, teta);
    screen_surface = display_image(destination);
    wait_for_keypressed();

    croped = crop(destination);
    if(croped)
    {
        screen_surface = display_image(croped);
        SDL_SaveBMP(croped, "croped.bmp");
        wait_for_keypressed();
        croped = colorationCrop(croped);
        SDL_SaveBMP(croped, "grid_only.bmp");
        lines(croped);
        SDL_SaveBMP(croped, "lines.bmp");
        screen_surface = display_image(croped);
    }
    else
    {
        SDL_SaveBMP(destination, "croped.bmp");
        SDL_SaveBMP(destination, "grid_only.bmp");
        croped = coloration(destination);
        SDL_SaveBMP(croped, "lines.bmp");
        screen_surface = display_image(destination);
    }

    wait_for_keypressed();
    SDL_FreeSurface(croped);
    SDL_FreeSurface(destination);
    SDL_FreeSurface(image_surface);
    SDL_FreeSurface(screen_surface);

    return 0;
}*/