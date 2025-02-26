#include"src/game.h"

#include"src/config.h"
#include"src/player.h"
#include"src/enemy.h"

#include"src/graphics.h"


#include<algorithm>

int score;
int currentFPS = 0;

void handleEnemyAndArrow() {
    for (int i = 0; i < enemies.size();) {
        SDL_Rect rectEnemy = enemies[i].rect;
        rectEnemy.x -= rectEnemy.w / 2;
        rectEnemy.y -= rectEnemy.h / 2;
        bool hit = false;
        for (int j = 0; j < arrows.size();) {
            SDL_Rect rectArrow = arrows[j].rect;
            rectArrow.x -= rectArrow.w / 2;
            rectArrow.y -= rectArrow.h / 2;
            if (SDL_HasIntersection(&rectEnemy, &rectArrow)) {
                enemies[i].currentHP -= arrows[j].dame;
                arrows.erase(arrows.begin() + j);

                if (enemies[i].currentHP <= 0) {

                    score += enemies[i].type * 5;
                    
                    enemies.erase(enemies.begin() + i);
                    hit = true;
                    break;
                }

            } else {
                j++;
            }
        }
        if (!hit) {
            i++;
        }
    }
}

void handleSpcecilEnemyAndArrow() {
    SDL_Rect rectEnemy = specialEnemy.rect;
    rectEnemy.x -= rectEnemy.w / 2;
    rectEnemy.y -= rectEnemy.h / 2;
    for (int j = 0; j < arrows.size();) {
        SDL_Rect rectArrow = arrows[j].rect;
        rectArrow.x -= rectArrow.w / 2;
        rectArrow.y -= rectArrow.h / 2;
        if (SDL_HasIntersection(&rectEnemy, &rectArrow)) {
            specialEnemy.currentHP -= arrows[j].dame;
            arrows.erase(arrows.begin() + j);

            if (specialEnemy.currentHP <= 0) {
                score += (1 << specialEnemy.level) * 15;
                break;
            }
        } else {
            j++;
        }
    }
}

void handlePlayerAndEnemy() {
    SDL_Rect rectPlayer = player.rect;
    rectPlayer.x -= rectPlayer.w / 2;
    rectPlayer.y -= rectPlayer.h / 2;
    for (int i = 0; i < enemies.size(); i++) {
        SDL_Rect rectEnemy = enemies[i].rect;
        rectEnemy.x -= rectEnemy.w / 2;
        rectEnemy.y -= rectEnemy.h / 2;
        if (SDL_HasIntersection(&rectPlayer, &rectEnemy)) {
            player.currentHP -= enemies[i].dame;
            enemies[i].currentHP -= player.dame;
            if (enemies[i].currentHP <= 0) {
                score += enemies[i].type * 5;
                enemies.erase(enemies.begin() + i);
            }
        }
    }
}

void handlePlayerAndSpecialEnemy() {
    SDL_Rect rectPlayer = player.rect;
    SDL_Rect rectEnemy = specialEnemy.rect;

    rectPlayer.x -= rectPlayer.w / 2;
    rectPlayer.y -= rectPlayer.h / 2;
    rectEnemy.x -= rectEnemy.w / 2;
    rectEnemy.y -= rectEnemy.h / 2;

    if (SDL_HasIntersection(&rectPlayer, &rectEnemy)) {
        player.currentHP -= specialEnemy.dame;
        specialEnemy.currentHP -= player.dame;
    }
}

void renderGraphicsSDL(SDL_Renderer** renderer) {
    static Uint32 lastTime = 0;
    Uint32 frameStart = SDL_GetTicks(); // Lấy thời gian bắt đầu khung hình

    // --- Vẽ nội dung ---
    SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 255);
    SDL_RenderClear(*renderer);

    drawFence(renderer, player.rect.x, player.rect.y);
    drawGrass(renderer, player.rect.x, player.rect.y);

    drawCircle(renderer, 0, 0, player.rect.w / 2, player.color, player.maxHP, player.currentHP);


    for (auto arrow : arrows) {
        arrow.rect.x -= player.rect.x;
        arrow.rect.y -= player.rect.y;
        drawCircle(renderer, arrow.rect.x, arrow.rect.y, arrow.rect.w / 2, arrow.color);
    }
    SDL_Rect rectSpecialEnemy = specialEnemy.rect;
    rectSpecialEnemy.x -= player.rect.x;
    rectSpecialEnemy.y -= player.rect.y;
    drawRegularPolygon(renderer, rectSpecialEnemy, specialEnemy.color, specialEnemy.level / 2 + 3, 1, specialEnemy.maxHP, specialEnemy.currentHP);

    for (auto enemy : enemies) {
        enemy.rect.x -= player.rect.x;
        enemy.rect.y -= player.rect.y;
        drawRectangle(renderer, enemy.rect, enemy.color, 1, enemy.maxHP, enemy.currentHP);
    }

    drawScore(renderer, score);
    drawFPS(renderer, currentFPS);
    drawNumberEnemy(renderer, enemies.size());
    drawMiniMap(renderer);

    SDL_RenderPresent(*renderer);

    // --- Giới hạn FPS ---
    Uint32 frameTime = SDL_GetTicks() - frameStart;
    const Uint32 frameDelay = 1000 / 60; // ~16.67ms cho 60 FPS

    if (frameTime < frameDelay) {
        SDL_Delay(frameDelay - frameTime); // Delay nếu frame kết thúc sớm
    }

    lastTime = frameStart;
}

int handleGame(SDL_Window** window, SDL_Renderer** renderer) {

    SDL_Event event;
    bool running = true;
    score = 0;
    int frameCount = 0, lastTime = 0;
    const Uint8* state = nullptr;
    Uint32 mouseState;

    arrows.clear();
    enemies.clear();

    initPlayer();
    initSpecialEnemy();
    drawGrass(renderer, player.rect.x, player.rect.y, 1);
    drawFence(renderer, player.rect.x, player.rect.y, 1);

    

    while (running) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return 0;
            }
        }
        state = SDL_GetKeyboardState(NULL);
        mouseState = SDL_GetMouseState(NULL, NULL);

        frameCount++;
        Uint32 currentTime = SDL_GetTicks();

        if (currentTime - lastTime >= 1000) {
            // Cập nhật FPS bằng số frame đã đếm được
            currentFPS = frameCount;
            
            // Reset bộ đếm frame và cập nhật lại thời gian
            frameCount = 0;
            lastTime = currentTime;
        }

        handlePlayerAction(state, mouseState);
        handleEnemyAction();

        for (auto& arrow: arrows) {
            arrow.rect.x += arrow.dx;
            arrow.rect.y += arrow.dy;
        }

        arrows.erase(std::remove_if(arrows.begin(), arrows.end(), [](GameObject& arrow) {
            return !(arrow.rect.x * arrow.rect.x + arrow.rect.y * arrow.rect.y <= (WORLD_MAP_WIDTH / 2) * (WORLD_MAP_WIDTH / 2));
        }), arrows.end());
        
        handleEnemyAndArrow();
        handleSpcecilEnemyAndArrow();

        if (specialEnemy.currentHP <= 0) {
            initSpecialEnemy();
        }

        renderGraphicsSDL(renderer);

        handlePlayerAndEnemy();
        handlePlayerAndSpecialEnemy();

        if (player.currentHP <= 0) {
            running = false;
        }
        if (specialEnemy.currentHP <= 0) {
            initSpecialEnemy();
        }
    }
    
    return score;
}
