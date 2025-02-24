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

    // Sinh góc và bán kính ngẫu nhiên
    double angle = ((double)rand() / RAND_MAX) * 2 * M_PI; // Góc từ 0 đến 2π
    double radius = 800 + ((double)rand() / RAND_MAX) * 200; // Bán kính từ 800 đến 1000

    // Tính tọa độ theo góc và bán kính
    int spawnX = radius * cos(angle);
    int spawnY = radius * sin(angle);

    SDL_Rect rect = {spawnX - 17, spawnY - 17, 35, 35}; // Căn giữa enemy

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
        float deltaX = player.rect.x - enemy.rect.x;
        float deltaY = player.rect.y - enemy.rect.y;
        float length = sqrt(deltaX * deltaX + deltaY * deltaY);
        if (length + 2 <= player.rect.w) {
            continue;
        }
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