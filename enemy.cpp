#include"src/enemy.h"
#include"src/player.h"
#include<iostream>

std::vector<GameObject> enemies;

void enemySpawn() {
    int enemyType = rand() % 3 + 1;

    SDL_Color enemyColor;

    switch (enemyType) {
        case 1:
            enemyColor = {225, 0, 0, 255};
            break;
        case 2:
            enemyColor = {0, 255, 0, 255};
            break;
        case 3:
            enemyColor = {0, 0, 255, 255};
            break;
    }

    SDL_Rect rect;

    switch (rand() % 10 % 4) {
        case 0:
            rect = {rand() % WINDOW_WIDTH - 50, 0, 35, 35};
            break;
        case 1:
            rect = {0, rand() % WINDOW_HEIGHT, 35, 35};
            break;
        case 2:
            rect = {WINDOW_WIDTH, rand() % WINDOW_HEIGHT, 35, 35};
            break;
        case 3:
            rect = {rand() % WINDOW_WIDTH, WINDOW_HEIGHT, 35, 35};
            break;
    }
    

    GameObject enemy = {
        rect,
        enemyColor,
        0, 0,
        enemyType, (8 - enemyType * 2), (8 - enemyType * 2), (8 - enemyType * 2)
    };
    enemies.push_back(enemy);
}

void enemyAction() {
    for (GameObject& enemy : enemies) {
        float deltaX = (player.rect.x + player.rect.w / 2) - (enemy.rect.x + enemy.rect.w / 2);
        float deltaY = (player.rect.y + player.rect.h / 2) - (enemy.rect.y + enemy.rect.h / 2);
        float length = sqrt(deltaX * deltaX + deltaY * deltaY);

        int speed = 0;
        switch (enemy.type) {
            case 1: speed = 3; break; 
            case 2: speed = 5; break; 
            case 3: speed = 7; break; 
            default: speed = 5;  
        }
    
        enemy.rect.x += deltaX / length * speed;
        enemy.rect.y += deltaY / length * speed;

    }
}

void handleEnemyAction() {
    if (rand() % 16 == 0) {
        enemySpawn();
    }

    enemyAction();
}