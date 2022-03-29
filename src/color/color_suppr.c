#include <err.h>
#include <math.h>
#include <gtk/gtk.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_rotozoom.h"
#include "color_suppr.h"
#include "../utils/sdl_utils.h"

int adapt_thresholding_mean(SDL_Surface* image_surface, int x, int y, int width, int height)
{
    int size = 50;
    int nb_pixels = 0;
    int sum = 0;
    for (size_t i = y; i < height && i < y + size; i++)
    {
        for (size_t j = x; j < width && j < x + size; j++)
        {
            Uint32 pixel = get_pixel(image_surface, j, i);
            Uint8 r, g, b;
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
            sum += r;
            nb_pixels++;
        }
    }
    return sum / nb_pixels;
}

void invert(SDL_Surface* image_surface)
{
    int width = image_surface->w;
    int height = image_surface->h;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            Uint8 r, g, b;
            Uint32 pixel = get_pixel(image_surface, x, y);
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
            Uint8 invert = 255 - r;
            pixel = SDL_MapRGB(image_surface->format, invert, invert, invert);
            put_pixel(image_surface, x, y, pixel);
        }
    }
}

void blackNwhite(SDL_Surface* image_surface)
{
    int threshold = 128;
    int width = image_surface->w;
    int height = image_surface->h;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            Uint8 r, g, b;
            Uint32 pixel = get_pixel(image_surface, x, y);
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);

            if ((x % 50 == 0 && x + 50 < width) || (y % 50 == 0 && y + 50 < height))
                threshold = adapt_thresholding_mean(image_surface, x, y, width, height);
            else
                threshold = 128;
            if (r >= threshold)
            {
                //printf("r > thres; x :%d; y :%d\n", x, y);
                pixel = SDL_MapRGB(image_surface->format, 255, 255, 255);
            }
            else
            {
                //printf("r < thres; x :%d; y :%d\n", x, y);
                pixel = SDL_MapRGB(image_surface->format, 0, 0, 0);
            }
            //pixel = SDL_MapRGB(image_surface->format, threshold, threshold, threshold);
            //printf("x :%d; y :%d; thres :%d\n", x, y, threshold);
            
            put_pixel(image_surface, x, y, pixel);
        }
    }
}


Uint8 check_new_contrast(Uint8 pixel_color, double contrast_correction)
{
    if((contrast_correction * (pixel_color - 128) + 128) > 255)
        return 255;
    else if((contrast_correction * (pixel_color - 128) + 128) < 0)
        return 0;
    else
        return (contrast_correction * (pixel_color - 128) + 128);
}

void contrast_change(SDL_Surface* image_surface, int change)
{
    int width = image_surface->w;
    int height = image_surface->h;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            Uint8 r, g, b;
            Uint8 newRed, newGreen, newBlue;
            Uint32 pixel = get_pixel(image_surface, x, y);
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
            
            double contrast_correction = (259 * (change + 255)) / (255 * (259 - change));
            newRed = check_new_contrast(r, contrast_correction);
            newGreen = check_new_contrast(g, contrast_correction);
            newBlue = check_new_contrast(b, contrast_correction);

            pixel = SDL_MapRGB(image_surface->format, newRed, newGreen, newBlue);

            put_pixel(image_surface, x, y, pixel);
        }
    }
}

int global_illumination(SDL_Surface* image_surface)
{
    int sum = 0;
    int width = image_surface->w;
    int height = image_surface->h;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            Uint8 r, g, b;
            Uint32 pixel = get_pixel(image_surface, x, y);
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
            sum += r;
        }
    }
    return sum / (width * height);
}

Uint8 check_max_value(Uint8 pixel_color, int change)
{
    if(pixel_color + change > 255)
        return 255;
    else if(pixel_color + change < 0)
        return 0;
    else
        return pixel_color + change;
}

void up_blacks(SDL_Surface* image_surface)
{
    int threshold = global_illumination(image_surface);
    int width = image_surface->w;
    int height = image_surface->h;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            Uint8 r, g, b;
            Uint8 newAverage;
            Uint32 pixel = get_pixel(image_surface, x, y);
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);

            if(r < threshold - 40)
            {
                newAverage = check_max_value(r, -50);
            }
            else
            {
                newAverage = check_max_value(r, 50);
            }

            pixel = SDL_MapRGB(image_surface->format, newAverage, newAverage, newAverage);

            put_pixel(image_surface, x, y, pixel);
        }
    }
}

Uint8 new_bright(Uint8 pixel, int change)
{
    return (Uint8) (255 * pow((double) pixel / 255, change)) % 255;
}

void brightness_change(SDL_Surface* image_surface, double change)
{
    int width = image_surface->w;
    int height = image_surface->h;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            Uint8 r, g, b;
            Uint8 newRed, newGreen, newBlue;
            Uint32 pixel = get_pixel(image_surface, x, y);
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
            
            newRed = new_bright(r, change);
            newGreen = new_bright(g, change);
            newBlue = new_bright(b, change);
            pixel = SDL_MapRGB(image_surface->format, newRed, newGreen, newBlue);

            put_pixel(image_surface, x, y, pixel);
        }
    }
}

void greyscale(SDL_Surface* image_surface)
{
    int width = image_surface->w;
    int height = image_surface->h;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            Uint8 r, g, b;
            Uint32 pixel = get_pixel(image_surface, x, y);
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
            Uint8 average = 0.3*r + 0.59*g + 0.11*b;
            pixel = SDL_MapRGB(image_surface->format, average, average, average);
            put_pixel(image_surface, x, y, pixel);
        }
    }
}

SDL_Surface* resize(SDL_Surface *img)
{
    int new_width = img->w;
    int new_height = img->h;

    while (new_width > 1000 || new_height > 1000)
    {
        new_width -= 100;
        new_height -= 100;
    }

    double ratio = SDL_max((double)new_height / (double)img->h, (double)new_width / (double)img->w);
    SDL_Surface* resized_img = rotozoomSurface(img, 0, ratio, 0);
    SDL_FreeSurface(img);
    return resized_img;
}

void gaussian_blur(SDL_Surface* image_surface, int level)
{
    int width = image_surface->w;
    int height = image_surface->h;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            box_gaussian_blur(image_surface, x, y, level);
        }
    }
}

void box_gaussian_blur(SDL_Surface* image_surface, int x_pixel, int y_pixel, int level)
{
    int x_start_box = SDL_max(x_pixel - level, 0);
    int y_start_box = SDL_max(y_pixel - level, 0);
    int nb_pixel = 0;
    Uint32 sum_r = 0;
    Uint32 sum_g = 0;
    Uint32 sum_b = 0;
    for (int y = y_start_box; y < image_surface->h && y <= y_pixel + level; y++)
    {
        for (int x = x_start_box; x < image_surface->w && x <= x_pixel + level; x++)
        {
            Uint8 r, g, b;
            Uint32 pixel = get_pixel(image_surface, x, y);
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
            sum_r += r;
            sum_g += g;
            sum_b += b;
            nb_pixel++;
        }
    }
    Uint32 pixel = SDL_MapRGB(image_surface->format, sum_r / nb_pixel, sum_g / nb_pixel, sum_b / nb_pixel);
    put_pixel(image_surface, x_pixel, y_pixel, pixel);
}