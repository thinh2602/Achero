#include"src/game.h"

#include"src/config.h"
#include"src/player.h"
#include"src/enemy.h"

#include"src/graphics.h"


#include<algorithm>

int score;
int currentFPS = 0;

void handleEnemyAndArrow() {
    for (auto &[enemyType, vectorEnemy] : enemies) {
        vectorEnemy.erase(std::remove_if(vectorEnemy.begin(), vectorEnemy.end(), [&](GameObject &enemy) {  // Thêm dấu & vào GameObject &enemy
            SDL_Rect rectEnemy = enemy.rect;
            rectEnemy.x -= rectEnemy.w / 2;
            rectEnemy.y -= rectEnemy.h / 2;
            
            arrows.erase(std::remove_if(arrows.begin(), arrows.end(), [&](GameObject &arrow) {  // Thêm dấu & vào GameObject &arrow
                if (enemy.currentHP <= 0) {
                    return false;
                }
                SDL_Rect rectArrow = arrow.rect;
                rectArrow.x -= rectArrow.w / 2;
                rectArrow.y -= rectArrow.h / 2;
                
                if (SDL_HasIntersection(&rectEnemy, &rectArrow)) {
                    enemy.currentHP -= arrow.dame;
                    return true;
                }
                return false;
            }), arrows.end());
            if (enemy.currentHP <= 0) {
                score += (1 << enemy.level) * 5;
            }
            return enemy.currentHP <= 0;
        }), vectorEnemy.end());
    }
}

void handlePlayerAndEnemy() {
    SDL_Rect rectPlayer = player.rect;
    rectPlayer.x -= rectPlayer.w / 2;
    rectPlayer.y -= rectPlayer.h / 2;

    for (auto& [enemyType, vectorEnemy] : enemies) {
        vectorEnemy.erase(std::remove_if(vectorEnemy.begin(), vectorEnemy.end(), [&](GameObject& enemy) {
            SDL_Rect rectEnemy = enemy.rect;
            rectEnemy.x -= rectEnemy.w / 2;
            rectEnemy.y -= rectEnemy.h / 2;

            if (SDL_HasIntersection(&rectPlayer, &rectEnemy)) {
                // enemy.currentHP -= player.dame;
                // player.currentHP -= enemy.dame;
                if (enemy.currentHP <= 0) {
                    score += (1 << enemy.level) * 5;
                    return true;
                }
            }
            return false;
        }), vectorEnemy.end()); 
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

    for (auto& [enemyType, vectorEnemy]: enemies) {
        for (auto enemy: vectorEnemy) {
            enemy.rect.x -= player.rect.x;
            enemy.rect.y -= player.rect.y;
            if (enemyType == EnemyType::Epic) {
                drawRegularPolygon(renderer, enemy.rect, enemy.color, enemy.level / 2 + 3, 1, enemy.maxHP, enemy.currentHP);
            } else {
                drawRectangle(renderer, enemy.rect, enemy.color, 1, enemy.maxHP, enemy.currentHP);
            }
        }
    }

    drawScore(renderer, score);
    drawFPS(renderer, currentFPS);

    int countEnemy = 0;
    for (auto& [enemyType, vectorEnemy]: enemies) {
        countEnemy += vectorEnemy.size();
    }

    drawNumberEnemy(renderer, countEnemy);
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
    drawGrass(renderer, player.rect.x, player.rect.y, 1);
    drawFence(renderer, player.rect.x, player.rect.y, 1);

    

    while (running) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return -1;
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

        renderGraphicsSDL(renderer);

        handlePlayerAndEnemy();

        if (player.currentHP <= 0) {
            running = false;
        }
    }
    
    return score;
}
