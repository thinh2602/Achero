#ifndef PLAYER_H
#define PLAYER_H

#include<SDL.h>
#include"config.h"

extern GameObject player;

void initPlayer();
void handlePlayerAction();

#endif