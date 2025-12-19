#include "npc.hpp"
#include <random>
#include <cmath>

NPC::NPC(NPCType t, const std::string& _name, double _x, double _y)
    : type(t), name(_name), x(_x), y(_y), alive(true) {}

void NPC::setPosition(double newX, double newY) {
    std::lock_guard<std::mutex> lock(mtx);
    x = newX;
    y = newY;
}

double NPC::distanceTo(const NPC* other) const {
    std::lock_guard<std::mutex> lock(mtx);
    double dx = x - other->x;
    double dy = y - other->y;
    return std::sqrt(dx*dx + dy*dy);
}

bool NPC::isClose(const NPC* other, double distance) const {
    return distanceTo(other) <= distance;
}

void NPC::move() {
    std::lock_guard<std::mutex> lock(mtx);
    if (!alive) return;
    
    // Случайное движение
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(-10, 10);
    
    x += dist(gen);
    y += dist(gen);
    
    // Проверка границ
    if (x < 0) x = 0;
    if (x > 99) x = 99;
    if (y < 0) y = 0;
    if (y > 99) y = 99;
}

void NPC::subscribe(std::shared_ptr<IFightObserver> observer) {
    observers.push_back(observer);
}

void NPC::fight_notify(std::shared_ptr<NPC> defender, bool win) {
    for (auto& obs : observers) {
        obs->on_fight(shared_from_this(), defender, win);
    }
}

int NPC::rollDice() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dist(1, 6);
    return dist(gen);
}