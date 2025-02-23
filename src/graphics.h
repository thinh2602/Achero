#ifndef graphics_H
#define graphics_H

#include<SDL.h>

int initSDL(SDL_Window** window, SDL_Renderer** renderer, const char *title, int width, int height);
bool renderEndGameScreen(SDL_Renderer** renderer);
void drawCircle(SDL_Renderer** renderer, int centreX, int centreY, int outerRadius, SDL_Color color, const int maxHP = 100, int currentHP = 100);
void drawRectangle(SDL_Renderer** renderer, SDL_Rect rect,  SDL_Color color, int hp = 100);
void drawGrass(SDL_Renderer** renderer);

#endif