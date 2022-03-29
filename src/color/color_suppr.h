#ifndef COLOR_SUPPR_H
#define COLOR_SUPPR_H

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"

int adapt_thresholding_mean(SDL_Surface* image_surface, int x_index, int y_index, int width, int height);
void invert(SDL_Surface* image_surface);
void blackNwhite(SDL_Surface* image_surface);
Uint8 check_new_contrast(Uint8 pixel_color, double contrast_correction);
void contrast_change(SDL_Surface* image_surface, int change);
int global_illumination(SDL_Surface* image_surface);
Uint8 check_max_value(Uint8 pixel_color, int change);
void up_blacks(SDL_Surface* image_surface);
Uint8 new_bright(Uint8 pixel, int change);
void brightness_change(SDL_Surface* image_surface, double change);
void greyscale(SDL_Surface* image_surface);
SDL_Surface* resize(SDL_Surface *img);
void gaussian_blur(SDL_Surface* image_surface, int level);
void box_gaussian_blur(SDL_Surface* image_surface, int x_pixel, int y_pixel, int level);

#endif