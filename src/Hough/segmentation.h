#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include <SDL/SDL.h>

void getlines(SDL_Surface *image, int *x, int *y);
float HoughTransform(SDL_Surface *image);
SDL_Surface* SDL_RotationCentralN(SDL_Surface* origine, float angle);
SDL_Surface* crop(SDL_Surface *image);
SDL_Surface* colorationCrop(SDL_Surface *image);
void lines(SDL_Surface *image);

#endif