#ifndef GRAPHICS_H
#define GRAPHICS_H

#include"config.h"
#include<SDL.h>
#include<vector>

int initSDL(SDL_Window** window, SDL_Renderer** renderer, const char* title, int width = WINDOW_WIDTH, int height = WINDOW_HEIGHT);
void drawCircle(SDL_Renderer** renderer, int centreX, int centreY, int outerRadius, SDL_Color color, const int maxHP = 1, int currentHP = 1);
void drawRectangle(SDL_Renderer** renderer, SDL_Rect rect,  SDL_Color color, int type = 0, int maxHP = 1, int currentHP = 1);
void drawGrass(SDL_Renderer** renderer, int originX, int originY, int type = 0);
void drawScore(SDL_Renderer** renderer, int score);
void drawNumberEnemy(SDL_Renderer** renderer, int numberEnemy);
void drawFence(SDL_Renderer** renderer, int originX, int originY, int type = 0);
void drawMiniMap(SDL_Renderer** renderer, const std::vector<GameObject>& enemies, const GameObject& player);
void drawFPS(SDL_Renderer** renderer, int currentFPS);
bool renderEndGameScreen(SDL_Renderer** renderer, int score);
void deInitSDL(SDL_Window** window, SDL_Renderer** renderer);


#endif


