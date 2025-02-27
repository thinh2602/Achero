#include"src/enemy.h"
#include<iostream>
#include<algorithm>

int epicEnemyCount = 2;

void spawnEnemy() {
    int type = rand() % 4 + 1;

    if (type == 4 && enemies[EnemyType::Epic].size() == epicEnemyCount) {
        return;
    }

    EnemyType enemyType;
    SDL_Color enemyColor;
    int speed, maxHP, dame;

    switch (type) {
        case 1: 
            enemyColor = {225, 0, 0, 255};
            enemyType = EnemyType::Normal;
            speed = 3;
            maxHP = 6;
            dame = 1;  
            break; 
        case 2: 
            enemyColor = {0, 255, 0, 255};
            enemyType = EnemyType::Normal;
            speed = 5;
            maxHP = 4;
            dame = 1;
            break; 
        case 3: 
            enemyColor = {0, 0, 255, 255};
            enemyType = EnemyType::Normal;
            speed = 7;
            maxHP = 2;
            dame = 1;  
            break; 
        default: 
            enemyColor = {128, 0, 128, 255};
            enemyType = EnemyType::Epic;
            speed = 10;
            maxHP = 10;   
            dame = 1; 
            break;
    }

    // Sinh góc và bán kính ngẫu nhiên
    double angle = ((double)rand() / RAND_MAX) * 2 * M_PI; // Góc từ 0 đến 2π
    double radius = (WORLD_MAP_WIDTH / 2 - 400) + ((double)rand() / RAND_MAX) * 400; // Bán kính từ 800 đến 1000

    // Tính tọa độ theo góc và bán kính
    int spawnX = radius * cos(angle);
    int spawnY = radius * sin(angle);

    SDL_Rect rect = {spawnX, spawnY, 35, 35};

    GameObject enemy = {
        rect,
        enemyColor,
        0, 0, 
        1, 0,
        speed, 
        maxHP, maxHP, 
        dame
    };

    enemies[enemyType].push_back(enemy);
}

void actionEnemy() {
    for (auto &[enemyType, vectorEnemy] : enemies) {
        for (auto &enemy : vectorEnemy) {
            float deltaX = player.rect.x - enemy.rect.x;
            float deltaY = player.rect.y - enemy.rect.y;
            float lengthSquared = deltaX * deltaX + deltaY * deltaY;

            if (enemyType == EnemyType::Epic && !(0 < lengthSquared && lengthSquared * 4 <= WINDOW_WIDTH * WINDOW_WIDTH)) {
                for (auto &normalEnemy : enemies[EnemyType::Normal]) { // Sửa: dùng tham chiếu
                    float dx = normalEnemy.rect.x - enemy.rect.x;
                    float dy = normalEnemy.rect.y - enemy.rect.y;
                    float lenSquared = dx * dx + dy * dy; // Sửa: dùng dx, dy thay vì deltaX, deltaY

                    if (lenSquared < lengthSquared) {
                        deltaX = dx;
                        deltaY = dy;
                        lengthSquared = lenSquared;
                    }
                }
            } else if (0 < lengthSquared && lengthSquared <= WINDOW_WIDTH * WINDOW_WIDTH) {
                float length = std::sqrt(lengthSquared); // Lấy căn bậc hai để so sánh khoảng cách thực
                if (length * 2 <= enemy.rect.w + player.rect.w) {
                    return;
                }
            }

            float length = std::sqrt(lengthSquared); // Lấy căn bậc hai trước khi chia
            if (length == 0) continue; // Tránh chia cho 0

            enemy.rect.x += (deltaX / length) * enemy.speed;
            enemy.rect.y += (deltaY / length) * enemy.speed;            
        }
    }
}


void enemyUpgrade(GameObject& enemy) {
    if ((1 << (enemy.level - 1)) <= enemy.xp) {
        enemy.xp -= (1 << (enemy.level - 1));
        enemy.level += 1;
        enemy.rect.w += enemy.rect.w / 3;
        enemy.rect.h += enemy.rect.h / 3;
        enemy.dx++;
        enemy.dy++;
        enemy.currentHP += enemy.maxHP / 3;
        enemy.maxHP += enemy.maxHP / 3;
        enemy.dame++;
    }
}

void enemyConsumeEnemy() {
    for (auto& epicEnemy : enemies[EnemyType::Epic]) {
        SDL_Rect rectEpicEnemy = epicEnemy.rect;
        rectEpicEnemy.x -= rectEpicEnemy.w / 2;
        rectEpicEnemy.y -= rectEpicEnemy.h / 2;
    
        std::vector<GameObject>& normalEnemies = enemies[EnemyType::Normal];
        normalEnemies.erase(std::remove_if(normalEnemies.begin(), normalEnemies.end(), [&](GameObject &normalEnemy) { 
            normalEnemy.rect.x -= normalEnemy.rect.w / 2;
            normalEnemy.rect.y -= normalEnemy.rect.h / 2;
            
            if (SDL_HasIntersection(&rectEpicEnemy, &normalEnemy.rect)) {
                epicEnemy.xp++;
                epicEnemy.currentHP = std::min(epicEnemy.currentHP + 2, epicEnemy.maxHP);
                enemyUpgrade(epicEnemy);
                return true;
            }
            normalEnemy.rect.x += normalEnemy.rect.w / 2;
            normalEnemy.rect.y += normalEnemy.rect.h / 2;
            return false;
        }), normalEnemies.end());  // Sửa: thêm `.end()`
    }
    
}

void handleEnemyAction() {

    if (rand() % 6 == 0) {
        spawnEnemy();
    }

    enemyConsumeEnemy();
    actionEnemy();
}