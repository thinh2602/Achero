#include"src/config.h"

const int WINDOW_WIDTH = 1056;
const int WINDOW_HEIGHT = 789;
const int WORLD_MAP_WIDTH = 3000;
const int WORLD_MAP_HEIGHT = 3000;
const int PLAYER_SPEED = 5;
const int ARROW_SPEED = 10;
const int WORLD_ORIGIN_X = WINDOW_WIDTH / 2;
const int WORLD_ORIGIN_Y = WINDOW_HEIGHT / 2;

std::vector<GameObject> arrows;
std::vector<GameObject> enemies;
GameObject player;
GameObject specialEnemy;
