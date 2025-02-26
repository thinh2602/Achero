#ifndef CONFIG_H
#define CONFIG_H

#include<SDL.h>
#include<vector>
#include<cstdlib>
#include<ctime>

#define WINDOW_TITLE "Achero"

extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;
extern const int PLAYER_SPEED;
extern const int ARROW_SPEED;
extern const int WORLD_ORIGIN_X;
extern const int WORLD_ORIGIN_Y;
extern const int WORLD_MAP_WIDTH;
extern const int WORLD_MAP_HEIGHT;

typedef struct {
    SDL_Rect rect;
    SDL_Color color;
    float dx, dy;
    int level, xp;
    int type;
    int currentHP, maxHP;
    int dame;
} GameObject;

extern std::vector<GameObject> arrows;
extern std::vector<GameObject> enemies;
extern GameObject player;
extern GameObject specialEnemy;

#endif