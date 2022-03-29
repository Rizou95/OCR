#ifndef SDL_UTILS_H
#define SDL_UTILS_H

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"

void init_sdl();
SDL_Surface* load_image(char *path);

SDL_Surface* display_image(SDL_Surface *img);

void wait_for_keypressed();

void SDL_FreeSurface(SDL_Surface *surface);

Uint8* pixel_ref(SDL_Surface *surf, unsigned x, unsigned y);

Uint32 get_pixel(SDL_Surface *surface, unsigned x, unsigned y);

void put_pixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel);

void update_surface(SDL_Surface* screen, SDL_Surface* image);

SDL_Surface *find_number(int number);

#endif