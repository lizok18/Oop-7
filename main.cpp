#include "npc.hpp"
#include "knight.hpp"
#include "dragon.hpp"
#include "pegasus.hpp"
#include "factory.hpp"
#include "observer.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <chrono>
#include <random>
#include <iomanip>
#include <algorithm>

using namespace std::chrono_literals;

// Константы игры
const int GAME_DURATION = 30;      // 30 секунд
const int NUM_NPCS = 50;           // 50 NPC
const int MAP_WIDTH = 100;
const int MAP_HEIGHT = 100;

// Глобальные переменные для управления игрой
std::vector<std::shared_ptr<NPC>> npcs;
std::mutex npcsMutex;
std::atomic<bool> gameRunning{false};
std::atomic<int> gameTime{0};

// Очередь для боев
struct CombatEvent {
    std::shared_ptr<NPC> attacker;
    std::shared_ptr<NPC> defender;
};

std::queue<CombatEvent> combatQueue;
std::mutex queueMutex;
std::condition_variable queueCV;

// Мьютекс для вывода в консоль
std::mutex coutMutex;

// Вспомогательные функции
int getKillDistance(NPCType type) {
    switch (type) {
        case NPCType::KNIGHT: return 10;
        case NPCType::DRAGON: return 30;
        case NPCType::PEGASUS: return 10;
        default: return 10;
    }
}

int getMoveDistance(NPCType type) {
    switch (type) {
        case NPCType::KNIGHT: return 30;
        case NPCType::DRAGON: return 50;
        case NPCType::PEGASUS: return 30;
        default: return 10;
    }
}

char getSymbol(NPCType type) {
    switch (type) {
        case NPCType::KNIGHT: return 'K';
        case NPCType::DRAGON: return 'D';
        case NPCType::PEGASUS: return 'P';
        default: return '?';
    }
}

// Создание начальных NPC
void createInitialNPCs() {
    std::lock_guard<std::mutex> lock(npcsMutex);
    std::lock_guard<std::mutex> coutLock(coutMutex);
    
    std::cout << "Создание " << NUM_NPCS << " NPC..." << std::endl;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> typeDist(1, 3);
    std::uniform_real_distribution<> xDist(0, MAP_WIDTH - 1);
    std::uniform_real_distribution<> yDist(0, MAP_HEIGHT - 1);
    
    for (int i = 0; i < NUM_NPCS; i++) {
        NPCType type = static_cast<NPCType>(typeDist(gen));
        double x = xDist(gen);
        double y = yDist(gen);
        std::string name = "NPC_" + std::to_string(i);
        
        auto npc = Factory::create(type, name, x, y);
        if (npc) {
            npcs.push_back(npc);
        }
    }
    
    std::cout << "Создано: " << npcs.size() << " NPC" << std::endl;
}

// Поток 1: Движение NPC
void movementWorker() {
    std::random_device rd;
    std::mt19937 gen(rd());
    
    {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << "[ПОТОК ДВИЖЕНИЯ] Запущен" << std::endl;
    }
    
    while (gameRunning && gameTime < GAME_DURATION) {
        // Получаем список живых NPC
        std::vector<std::shared_ptr<NPC>> aliveNPCs;
        {
            std::lock_guard<std::mutex> lock(npcsMutex);
            for (const auto& npc : npcs) {
                if (npc->isAlive()) {
                    aliveNPCs.push_back(npc);
                }
            }
        }
        
        // Двигаем каждого NPC
        for (auto& npc : aliveNPCs) {
            if (!gameRunning) break;
            
            NPCType type = npc->getType();
            int moveDist = getMoveDistance(type);
            
            // Генерируем случайное смещение
            std::uniform_int_distribution<> dist(-moveDist, moveDist);
            int dx = dist(gen);
            int dy = dist(gen);
            
            // Рассчитываем новую позицию (просто для примера)
            // В реальности нужно вызывать npc->move() или аналогичный метод
            
            // Проверяем врагов поблизости
            int killDist = getKillDistance(type);
            for (auto& other : aliveNPCs) {
                if (npc != other && other->isAlive()) {
                    double distance = npc->distanceTo(other.get());
                    if (distance <= killDist) {
                        // Добавляем бой в очередь
                        std::lock_guard<std::mutex> qlock(queueMutex);
                        combatQueue.push({npc, other});
                        queueCV.notify_one();
                    }
                }
            }
        }
        
        std::this_thread::sleep_for(200ms);
    }
    
    {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << "[ПОТОК ДВИЖЕНИЯ] Завершен" << std::endl;
    }
}

// Поток 2: Обработка боев
void combatWorker() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dice(1, 6);
    
    {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << "[ПОТОК БОЕВ] Запущен" << std::endl;
    }
    
    while (gameRunning) {
        CombatEvent event;
        bool hasEvent = false;
        
        // Берем событие из очереди
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCV.wait_for(lock, 100ms, [&]() {
                return !combatQueue.empty() || !gameRunning;
            });
            
            if (!combatQueue.empty()) {
                event = combatQueue.front();
                combatQueue.pop();
                hasEvent = true;
            }
        }
        
        if (hasEvent && event.attacker && event.defender &&
            event.attacker->isAlive() && event.defender->isAlive()) {
            
            // Бросаем кубики
            int attackRoll = dice(gen);
            int defenseRoll = dice(gen);
            
            // Используем Visitor паттерн для боя
            bool canKill = event.defender->accept(event.attacker);
            
            if (canKill && attackRoll > defenseRoll) {
                // Убиваем защитника
                event.defender->kill();
                
                // Выводим сообщение
                std::lock_guard<std::mutex> coutLock(coutMutex);
                std::cout << "[БОЙ] " << event.attacker->getName() 
                          << "(" << attackRoll << ") победил " 
                          << event.defender->getName() 
                          << "(" << defenseRoll << ")" << std::endl;
            }
        }
        
        // Если игра завершена и очередь пуста - выходим
        if (!gameRunning && combatQueue.empty()) {
            break;
        }
    }
    
    {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << "[ПОТОК БОЕВ] Завершен" << std::endl;
    }
}

// Функция для отображения карты
void printMap() {
    std::lock_guard<std::mutex> lock(npcsMutex);
    
    // Очистка экрана
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
    
    std::cout << "=== КАРТА ИГРЫ " << MAP_WIDTH << "x" << MAP_HEIGHT << " ===" << std::endl;
    std::cout << "Время: " << gameTime << "/" << GAME_DURATION << " секунд" << std::endl;
    
    // Считаем живых NPC
    int aliveCount = 0;
    for (const auto& npc : npcs) {
        if (npc->isAlive()) aliveCount++;
    }
    std::cout << "Живых NPC: " << aliveCount << "/" << npcs.size() << std::endl;
    std::cout << std::endl;
    
    // Создаем упрощенную карту 25x50
    const int DISPLAY_WIDTH = 50;
    const int DISPLAY_HEIGHT = 25;
    
    char map[DISPLAY_HEIGHT][DISPLAY_WIDTH];
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            map[y][x] = '.';
        }
    }
    
    // Размещаем NPC на карте
    for (const auto& npc : npcs) {
        if (npc->isAlive()) {
            int displayX = static_cast<int>((npc->getX() / MAP_WIDTH) * DISPLAY_WIDTH);
            int displayY = static_cast<int>((npc->getY() / MAP_HEIGHT) * DISPLAY_HEIGHT);
            
            if (displayX >= 0 && displayX < DISPLAY_WIDTH && 
                displayY >= 0 && displayY < DISPLAY_HEIGHT) {
                
                char symbol = getSymbol(npc->getType());
                map[displayY][displayX] = symbol;
            }
        }
    }
    
    // Рисуем карту
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            std::cout << map[y][x];
        }
        std::cout << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Легенда: K-Рыцарь, D-Дракон, P-Пегас, .-пусто" << std::endl;
    std::cout << "=============================================" << std::endl;
}

// Функция для вывода списка выживших
void printSurvivors() {
    std::lock_guard<std::mutex> lock(npcsMutex);
    
    std::cout << "\n=== ВЫЖИВШИЕ NPC ===" << std::endl;
    
    int count = 0;
    for (const auto& npc : npcs) {
        if (npc->isAlive()) {
            std::cout << "- ";
            npc->print();
            count++;
        }
    }
    
    if (count == 0) {
        std::cout << "Все NPC погибли!" << std::endl;
    } else {
        std::cout << "\nВсего выжило: " << count << " NPC" << std::endl;
    }
}

// Поток 3: Отображение (основной поток)
void displayWorker() {
    {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << "[ПОТОК ОТОБРАЖЕНИЯ] Запущен" << std::endl;
    }
    
    auto lastUpdate = std::chrono::steady_clock::now();
    
    while (gameRunning && gameTime < GAME_DURATION) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate);
        
        // Обновляем карту каждую секунду
        if (elapsed.count() >= 1000) {
            printMap();
            lastUpdate = now;
            gameTime++;
        }
        
        std::this_thread::sleep_for(100ms);
    }
    
    // Финальный вывод
    printMap();
    printSurvivors();
    
    {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << "[ПОТОК ОТОБРАЖЕНИЯ] Завершен" << std::endl;
    }
}

// Основная функция
int main() {
    try {
        std::cout << "=== NPC СИМУЛЯТОР ===" << std::endl;
        std::cout << "Три потока: Движение, Бои, Отображение" << std::endl;
        std::cout << "Длительность: " << GAME_DURATION << " секунд" << std::endl;
        std::cout << "Количество NPC: " << NUM_NPCS << std::endl;
        std::cout << "==============================" << std::endl;
        
        // Создаем NPC
        createInitialNPCs();
        
        // Запускаем игру
        gameRunning = true;
        gameTime = 0;
        
        // Запускаем три потока
        std::thread movementThread(movementWorker);
        std::thread combatThread(combatWorker);
        
        // Поток отображения работает в основном потоке
        displayWorker();
        
        // Останавливаем игру
        gameRunning = false;
        
        // Оповещаем все потоки
        queueCV.notify_all();
        
        // Ждем завершения потоков
        if (movementThread.joinable()) {
            movementThread.join();
            std::cout << "Поток движения завершен" << std::endl;
        }
        
        if (combatThread.joinable()) {
            combatThread.join();
            std::cout << "Поток боев завершен" << std::endl;
        }
        
        std::cout << "\n=== ИГРА ЗАВЕРШЕНА ===" << std::endl;
        std::cout << "Нажмите Enter для выхода...";
        std::cin.get();
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}