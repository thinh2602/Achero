#include"src/game.h"
#include"src/config.h"
#include"src/graphics.h"
#include"src/player.h"
#include"src/enemy.h"

#include<algorithm>

int score;

void handleEnemyAndArrow() {
    for (int i = 0; i < enemies.size();) {
        bool hit = false;
        for (int j = 0; j < arrows.size();) {
            if (SDL_HasIntersection(&enemies[i].rect, &arrows[j].rect)) {
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
    for (int i = 0; i < enemies.size(); i++) {
        if (SDL_HasIntersection(&player.rect, &enemies[i].rect)) {
            player.currentHP -= enemies[i].dame * 2;
            score += enemies[i].type * 5;
            enemies.erase(enemies.begin() + i);
        }
    }
}

int handleGame(SDL_Window** window, SDL_Renderer** renderer) {

    SDL_Event event;
    bool running = true;

    arrows.clear();
    enemies.clear();
    initPlayer();
    drawGrass(renderer, 1);

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
            return arrow.rect.x < 0 || arrow.rect.x > WINDOW_WIDTH || arrow.rect.y < 0 || arrow.rect.y > WINDOW_HEIGHT;
        }), arrows.end());
        
        handleEnemyAndArrow();

        SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 255);
        SDL_RenderClear(*renderer);

        drawGrass(renderer, 0);
    
        drawCircle(renderer, player.rect.x + player.rect.w / 2, player.rect.y + player.rect.h / 2, player.rect.w / 2, player.color, player.maxHP, player.currentHP);


        for (auto arrow: arrows) {
            drawCircle(renderer, arrow.rect.x + arrow.rect.w / 2, arrow.rect.y + arrow.rect.h / 2, arrow.rect.w / 2, arrow.color);
        }

        for (auto enemy : enemies) {
            drawRectangle(renderer, enemy.rect, enemy.color, enemy.maxHP, enemy.currentHP);
        }

        drawScore(renderer, score);
        
        SDL_RenderPresent(*renderer);
        SDL_Delay(16);

        handlePlayerAndEnemy();

        if (player.currentHP <= 0) {
            running = false;
        }
    }
    
    return score;
}
