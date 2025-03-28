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
    float angle = ((float)rand() / RAND_MAX) * 2 * M_PI; // Góc từ 0 đến 2π
    float radius = (WORLD_MAP_WIDTH / 2 - 400) + ((float)rand() / RAND_MAX) * 400; 

    // Tính tọa độ theo góc và bán kính
    float spawnX = radius * cos(angle);
    float spawnY = radius * sin(angle);

    SDL_Rect rect = {static_cast<int> (spawnX), static_cast<int> (spawnY), 35, 35};

    GameObject enemy = {
        rect,
        enemyColor,
        spawnX, spawnY, 
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
                    float lenSquared = dx * dx + dy * dy;

                    if (lenSquared < lengthSquared) {
                        deltaX = dx;
                        deltaY = dy;
                        lengthSquared = lenSquared;
                    }
                }
            } else if (0 <= lengthSquared && lengthSquared <= WINDOW_WIDTH * WINDOW_WIDTH) {
                float length = std::sqrt(lengthSquared); // Lấy căn bậc hai để so sánh khoảng cách thực
                if (length * 2 <= enemy.rect.w + player.rect.w - 2) {
                    continue;
                }
            } else {
                deltaX = enemy.dx - enemy.rect.x;
                deltaY = enemy.dy - enemy.rect.y;
                lengthSquared = deltaX * deltaX + deltaY * deltaY;
                if (lengthSquared <= enemy.rect.w * enemy.rect.w * 4) {
                    float angle = ((float)rand() / RAND_MAX) * 2 * M_PI; // Góc từ 0 đến 2π
                    float radius = (WORLD_MAP_WIDTH / 2 - 400) + ((float)rand() / RAND_MAX) * 400;
                    enemy.dx = radius * cos(angle);
                    enemy.dy = radius * sin(angle);
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
    for (auto &epicEnemy : enemies[EnemyType::Epic]) {
        SDL_Rect rectEpicEnemy = epicEnemy.rect;
        rectEpicEnemy.x -= rectEpicEnemy.w / 2;
        rectEpicEnemy.y -= rectEpicEnemy.h / 2;

        // Xử lý ăn NormalEnemy
        std::vector<GameObject> &normalEnemies = enemies[EnemyType::Normal];
        normalEnemies.erase(std::remove_if(normalEnemies.begin(), normalEnemies.end(), [&](GameObject &normalEnemy) {
            normalEnemy.rect.x -= normalEnemy.rect.w / 2;
            normalEnemy.rect.y -= normalEnemy.rect.h / 2;

            if (SDL_HasIntersection(&rectEpicEnemy, &normalEnemy.rect)) {
                int gainedXP = std::pow(2, normalEnemy.level - 1); // EXP = 2^(level - 1)
                epicEnemy.xp += gainedXP;
                epicEnemy.currentHP = std::min(epicEnemy.currentHP + 2, epicEnemy.maxHP);
                enemyUpgrade(epicEnemy);
                return true;
            }
            normalEnemy.rect.x += normalEnemy.rect.w / 2;
            normalEnemy.rect.y += normalEnemy.rect.h / 2;
            return false;
        }), normalEnemies.end());

        // Xử lý EpicEnemy tự ăn nhau nếu có level thấp hơn
        std::vector<GameObject> &epicEnemies = enemies[EnemyType::Epic];
        epicEnemies.erase(std::remove_if(epicEnemies.begin(), epicEnemies.end(), [&](GameObject &otherEpicEnemy) {
            if (&epicEnemy == &otherEpicEnemy) return false; // Không ăn chính mình
            
            SDL_Rect rectOther = otherEpicEnemy.rect;
            rectOther.x -= rectOther.w / 2;
            rectOther.y -= rectOther.h / 2;

            if (otherEpicEnemy.level < epicEnemy.level && SDL_HasIntersection(&rectEpicEnemy, &rectOther)) {
                int gainedXP = std::pow(2, otherEpicEnemy.level - 1); // EXP = 2^(level - 1)
                epicEnemy.xp += gainedXP;
                epicEnemy.currentHP = std::min(epicEnemy.currentHP + otherEpicEnemy.currentHP, epicEnemy.maxHP);
                enemyUpgrade(epicEnemy);
                return true;
            }
            return false;
        }), epicEnemies.end());
    }
}


void handleEnemyAction() {

    if (rand() % 6 == 0) {
        spawnEnemy();
    }

    enemyConsumeEnemy();
    actionEnemy();
}