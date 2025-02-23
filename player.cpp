#include"src/player.h"

GameObject player;

void initPlayer() {
    player = {
        {WINDOW_WIDTH / 2 - 25, WINDOW_HEIGHT / 2 - 25, 40, 40}, 
        {0, 255, 0, 255}, 
        0, 0, 0, 100, 100, 1
    };
}

void PlayerMovement() {
    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    if ((keystates[SDL_SCANCODE_A] || keystates[SDL_SCANCODE_LEFT]) && player.rect.x > 0) {
        player.rect.x -= PLAYER_SPEED;
    } 

    if ((keystates[SDL_SCANCODE_D] || keystates[SDL_SCANCODE_RIGHT]) && player.rect.x < WINDOW_WIDTH - player.rect.w) {
        player.rect.x += PLAYER_SPEED;
    }

    if ((keystates[SDL_SCANCODE_W] || keystates[SDL_SCANCODE_UP]) && player.rect.y > 0) {
        player.rect.y -= PLAYER_SPEED;
    }

    if ((keystates[SDL_SCANCODE_S] || keystates[SDL_SCANCODE_DOWN]) && player.rect.y < WINDOW_HEIGHT - player.rect.h) {
        player.rect.y += PLAYER_SPEED;
    }
}

void PlayerAction() {
    if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        float deltaX = mouseX - (player.rect.x + player.rect.w / 2);
        float deltaY = mouseY - (player.rect.y + player.rect.h / 2);
        float length = sqrt(deltaX * deltaX + deltaY * deltaY);
        if (length != 0) {
            GameObject arrow = {
                {player.rect.x + player.rect.w / 2 - 5, player.rect.y + player.rect.h / 2 - 5, 10, 10},
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