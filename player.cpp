#include"src/player.h"



void initPlayer() {
    player = {
        {0, 0, 40, 40}, 
        {0, 255, 0, 255}, 
        0, 0, 
        1, 0, 
        0,
        100, 100, 
        1
    };
}

SDL_Scancode keys[] = {SDL_SCANCODE_A, SDL_SCANCODE_D, SDL_SCANCODE_W, SDL_SCANCODE_S};

int dx[] = {-1, 1, 0, 0};
int dy[] = {0, 0, -1, 1};


void actionPlayer(const Uint32 mouseState) {
    if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) {
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
                1, 0,
                0,
                1, 1, 
                1
            };
            arrows.push_back(arrow);
        }
    }
}

void movementPlayer(const Uint8* state, float dashDistance) {
    float deltaX = 0.0f, deltaY = 0.0f;

    for (int i = 0; i < 4; i++) {
        if (state[keys[i]]) {
            deltaX += dx[i];
            deltaY += dy[i];
        }
    }

    // Tính độ dài vector di chuyển và chuẩn hóa
    float length = sqrtf(deltaX * deltaX + deltaY * deltaY);
    if (length == 0) {
        return;
    }

    float invLength = 1.0f / length;
    float moveX = deltaX * invLength * dashDistance;
    float moveY = deltaY * invLength * dashDistance;

    // Tính vị trí mới dựa trên chuyển động lướt
    float newX = player.rect.x + moveX;
    float newY = player.rect.y + moveY;

    // Tính khoảng cách bình phương từ vị trí mới đến tâm của hình tròn
    float distSquared = newX * newX + newY * newY;
    float maxRadius = WORLD_MAP_WIDTH * 0.5f;
    float maxRadiusSquared = maxRadius * maxRadius;

    if (distSquared <= maxRadiusSquared) {
        // Nếu vị trí mới nằm trong biên của hình tròn
        player.rect.x = newX;
        player.rect.y = newY;
    } else {
    }
}

void handlePlayerAction(const Uint8* state, const Uint32 mouseState) {

    if (state[SDL_SCANCODE_SPACE]) {
        movementPlayer(state, 25); // Gọi hàm lướt với khoảng cách 50
    } else {
        movementPlayer(state, PLAYER_SPEED);
    }

    actionPlayer(mouseState);

}