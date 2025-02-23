#ifndef GAME_H
#define GAME_H

#include<SDL.h>

void handleEnemyAndArrow();
int handleGame(SDL_Window** window, SDL_Renderer** renderer);
bool checkPlayerAndEnemy();

#endif