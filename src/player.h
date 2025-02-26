#ifndef PLAYER_H
#define PLAYER_H

#include<SDL.h>
#include"config.h"

extern GameObject player;

void initPlayer();
void handlePlayerAction(const Uint8* state, const Uint32 mouseState);

#endif