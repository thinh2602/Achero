#ifndef CONFIG_H
#define CONFIG_H

#include<SDL.h>
#include<vector>
#include<cstdlib>
#include<ctime>

#define WINDOW_TITLE "test"

extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;
extern const int PLAYER_SPEED;
extern const int ARROW_SPEED;
extern const int ENEMY_SPEED;

typedef struct {
    SDL_Rect rect;
    SDL_Color color;
    float dx, dy;
    int type;
    int currentHP, maxHP;
    int dame;
} GameObject;

extern std::vector<GameObject> arrows;

#endif