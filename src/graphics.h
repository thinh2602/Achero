#ifndef GRAPHICS_H
#define GRAPHICS_H

#include"config.h"
#include<SDL.h>
#include<vector>

int initSDL(SDL_Window** window, SDL_Renderer** renderer, const char *title, int width, int height);
void drawCircle(SDL_Renderer** renderer, int centreX, int centreY, int outerRadius, SDL_Color color, const int maxHP = 1, int currentHP = 1);
void drawRectangle(SDL_Renderer** renderer, SDL_Rect rect,  SDL_Color color, int maxHP = 1, int currentHP = 1);
void drawGrass(SDL_Renderer** renderer, int type = 0);
void drawScore(SDL_Renderer** renderer, int score);
bool renderEndGameScreen(SDL_Renderer** renderer, int score);
void drawMiniMap(SDL_Renderer** renderer, const std::vector<GameObject>& enemies, const GameObject& player);
void deInitSDL(SDL_Window** window, SDL_Renderer** renderer);


#endif


