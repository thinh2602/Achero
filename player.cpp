#include"src/player.h"

GameObject player;


void initPlayer() {
    player = {
        {0, 0, 40, 40}, 
        {0, 255, 0, 255}, 
        0, 0, 0, 100, 100, 1
    };
}

SDL_Scancode keys[] = {SDL_SCANCODE_A, SDL_SCANCODE_LEFT, SDL_SCANCODE_D, SDL_SCANCODE_RIGHT,
    SDL_SCANCODE_W, SDL_SCANCODE_UP, SDL_SCANCODE_S, SDL_SCANCODE_DOWN};

int dx[] = {-1, 1, 0, 0};
int dy[] = {0, 0, -1, 1};

void PlayerMovement() {
    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    
    for (int i = 0; i < 4; i++) {
        if (keystates[keys[2 * i]] || keystates[keys[2 * i + 1]]) {
            int playerX = player.rect.x + dx[i] * PLAYER_SPEED;
            int playerY = player.rect.y + dy[i] * PLAYER_SPEED;
            if (playerX * playerX + playerY * playerY <= 1000000) {
                player.rect.x = playerX;
                player.rect.y = playerY;
            }
        }
    }
}

void PlayerAction() {
    if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        mouseX -= WORLD_ORIGIN_X;
        mouseY -= WORLD_ORIGIN_Y;
        float deltaX = mouseX;
        float deltaY = mouseY;
        float length = sqrt(deltaX * deltaX + deltaY * deltaY);
        if (length != 0) {
            GameObject arrow = {
                {player.rect.x, player.rect.y, 10, 10},
                {255, 255, 0, 255},
                (deltaX / length) * ARROW_SPEED,
                (deltaY / length) * ARROW_SPEED,
                0, 1, 1, 1
            };
            arrows.push_back(arrow);
        }
    }
}

void handlePlayerAction() {
    PlayerMovement();
    PlayerAction();
}