#include"src/enemy.h"
#include<iostream>




void spawnEnemy() {
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
    double radius = (WORLD_MAP_WIDTH / 2 - 100) + ((double)rand() / RAND_MAX) * 100; // Bán kính từ 800 đến 1000

    // Tính tọa độ theo góc và bán kính
    int spawnX = radius * cos(angle);
    int spawnY = radius * sin(angle);

    SDL_Rect rect = {spawnX, spawnY, 35, 35};

    GameObject enemy = {
        rect,
        enemyColor,
        0, 0, 
        1, 0,
        enemyType, 
        (8 - enemyType * 2), (8 - enemyType * 2), 
        (8 - enemyType * 2)
    };

    enemies.push_back(enemy);
}

void actionEnemy() {
    for (GameObject& enemy : enemies) {
        float deltaX, deltaY, length;
        if ((player.rect.x - enemy.rect.x) * (player.rect.x - enemy.rect.x) + (player.rect.y - enemy.rect.y) * (player.rect.y - enemy.rect.y) <
        (specialEnemy.rect.x - enemy.rect.x + 50) * (specialEnemy.rect.x - enemy.rect.x + 50) + (specialEnemy.rect.y - enemy.rect.y + 50) * (specialEnemy.rect.y - enemy.rect.y + 50)) {
            deltaX = player.rect.x - enemy.rect.x;
            deltaY = player.rect.y - enemy.rect.y;
            length = sqrt(deltaX * deltaX + deltaY * deltaY);
            float length = sqrt(deltaX * deltaX + deltaY * deltaY);
        } else {
            deltaX = specialEnemy.rect.x - enemy.rect.x;
            deltaY = specialEnemy.rect.y - enemy.rect.y;
            length = sqrt(deltaX * deltaX + deltaY * deltaY);
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

void actionSpecialEnemy() {
    float deltaX = player.rect.x - specialEnemy.rect.x;
    float deltaY = player.rect.y - specialEnemy.rect.y;
    float length = sqrt(deltaX * deltaX + deltaY * deltaY);
    int speed = specialEnemy.dx;
    if (0 < length && length <= WINDOW_WIDTH / 2) {
        specialEnemy.rect.x += deltaX / length * speed;
        specialEnemy.rect.y += deltaY / length * speed;
    } else if ((int) enemies.size()){
        deltaX = enemies[0].rect.x - specialEnemy.rect.x;
        deltaY = enemies[0].rect.y - specialEnemy.rect.y;
        length = sqrt(deltaX * deltaX + deltaY * deltaY);
        if (length == 0) {
            return;
        }
        specialEnemy.rect.x += deltaX / length * speed;
        specialEnemy.rect.y += deltaY / length * speed;
    }

}

void initSpecialEnemy() {
    double angle = ((double)rand() / RAND_MAX) * 2 * M_PI;
    double radius = (WORLD_MAP_WIDTH / 2 - 400) + ((double)rand() / RAND_MAX) * 200;

    int spawnX = radius * cos(angle);
    int spawnY = radius * sin(angle);

    SDL_Rect rect = {spawnX, spawnY, 40, 40};

    specialEnemy = {
        rect,
        {128, 0, 128, 255},
        7, 7,
        1, 0, 
        0,
        20, 20,
        3
    };
}

void enemyUpgrade() {
    if ((1 << (specialEnemy.level - 1)) <= specialEnemy.xp) {
        specialEnemy.xp -= (1 << (specialEnemy.level - 1));
        specialEnemy.level += 1;
        specialEnemy.rect.w += specialEnemy.rect.w / 3;
        specialEnemy.rect.h += specialEnemy.rect.h / 3;
        specialEnemy.dx++;
        specialEnemy.dy++;
        specialEnemy.currentHP += specialEnemy.maxHP / 3;
        specialEnemy.maxHP += specialEnemy.maxHP / 3;
        specialEnemy.dame++;
    }
}

void specialEnemyConsumeEnemy() {
    SDL_Rect rectSpecialEnemy = specialEnemy.rect;
    rectSpecialEnemy.x -= rectSpecialEnemy.w / 2;
    rectSpecialEnemy.y -= rectSpecialEnemy.h / 2;
    for (int i = 0; i < enemies.size();) {
        SDL_Rect rectEnemy = enemies[i].rect;
        rectEnemy.x -= rectEnemy.w / 2;
        rectEnemy.y -= rectEnemy.h / 2;
        if (SDL_HasIntersection(&rectSpecialEnemy, &rectEnemy)) {
            specialEnemy.xp += 1 << (enemies[i].level - 1);
            specialEnemy.currentHP = std::min(specialEnemy.currentHP + 2, specialEnemy.maxHP);
            enemies.erase(enemies.begin() + i);
        } else {
            i++;
        }
    }
}

void handleEnemyAction() {
    specialEnemyConsumeEnemy();

    if (rand() % 16 == 0) {
        spawnEnemy();
    }

    enemyUpgrade();

    actionEnemy();
    actionSpecialEnemy();
}