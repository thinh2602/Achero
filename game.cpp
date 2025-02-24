#include"src/game.h"
#include"src/config.h"
#include"src/graphics.h"
#include"src/player.h"
#include"src/enemy.h"

#include<algorithm>

int score;

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

int handleGame(SDL_Window** window, SDL_Renderer** renderer) {

    SDL_Event event;
    bool running = true;

    arrows.clear();
    enemies.clear();
    initPlayer();
    drawGrass(renderer, player.rect.x, player.rect.y, 1);

    score = 0;

    while (running) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return 0;
            }
        }

        handlePlayerAction();
        handleEnemyAction();

        for (auto& arrow: arrows) {
            arrow.rect.x += arrow.dx;
            arrow.rect.y += arrow.dy;
        }

        arrows.erase(std::remove_if(arrows.begin(), arrows.end(), [](GameObject& arrow) {
            return !(arrow.rect.x * arrow.rect.x + arrow.rect.y * arrow.rect.y <= 1000000);
        }), arrows.end());
        
        handleEnemyAndArrow();

        SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 255);
        SDL_RenderClear(*renderer);

        drawGrass(renderer, player.rect.x, player.rect.y, 0);
    
        drawCircle(renderer, 0, 0, player.rect.w / 2, player.color, player.maxHP, player.currentHP);


        for (auto arrow: arrows) {
            int dx = arrow.rect.x - player.rect.x;
            int dy = arrow.rect.y - player.rect.y;
            drawCircle(renderer, dx, dy, arrow.rect.w / 2, arrow.color);
        }

        for (auto enemy : enemies) {
            enemy.rect.x -= player.rect.x;
            enemy.rect.y -= player.rect.y;
            drawRectangle(renderer, enemy.rect, enemy.color, 1, enemy.maxHP, enemy.currentHP);
        }

        drawScore(renderer, score);
        drawMiniMap(renderer, enemies, player);
        
        SDL_RenderPresent(*renderer);
        SDL_Delay(16);

        handlePlayerAndEnemy();

        if (player.currentHP <= 0) {
            running = false;
        }
    }
    
    return score;
}
