#include "dragon.hpp"
#include "knight.hpp"    // ← ДОБАВИТЬ
#include "pegasus.hpp"   // ← ДОБАВИТЬ
#include <iostream>

Dragon::Dragon(const std::string& name, double x, double y)
    : NPC(NPCType::DRAGON, name, x, y) {}

void Dragon::print() const {
    std::cout << "Дракон '" << name << "' (" << x << ", " << y << ") "
              << (alive ? "жив" : "мертв") << std::endl;
}

bool Dragon::accept(std::shared_ptr<NPC> attacker) {
    return attacker->fight(shared_from_this());
}

bool Dragon::fight(std::shared_ptr<NPC> other) {
    return other->visit(std::dynamic_pointer_cast<Dragon>(shared_from_this()));
}

bool Dragon::visit(std::shared_ptr<Knight> other) {
    fight_notify(other, false);
    return false;
}

bool Dragon::visit(std::shared_ptr<Dragon> other) {
    fight_notify(other, false);
    return false;
}

bool Dragon::visit(std::shared_ptr<Pegasus> other) {
    fight_notify(other, true);
    return true;
}