#include "knight.hpp"
#include "dragon.hpp"    // ← ДОБАВИТЬ
#include "pegasus.hpp"   // ← ДОБАВИТЬ
#include <iostream>

Knight::Knight(const std::string& name, double x, double y)
    : NPC(NPCType::KNIGHT, name, x, y) {}

void Knight::print() const {
    std::cout << "Рыцарь '" << name << "' (" << x << ", " << y << ") "
              << (alive ? "жив" : "мертв") << std::endl;
}

bool Knight::accept(std::shared_ptr<NPC> attacker) {
    return attacker->fight(shared_from_this());
}

bool Knight::fight(std::shared_ptr<NPC> other) {
    return other->visit(std::dynamic_pointer_cast<Knight>(shared_from_this()));
}

bool Knight::visit(std::shared_ptr<Knight> other) {
    fight_notify(other, false);
    return false;
}

bool Knight::visit(std::shared_ptr<Dragon> other) {
    fight_notify(other, true);
    return true;
}

bool Knight::visit(std::shared_ptr<Pegasus> other) {
    fight_notify(other, false);
    return false;
}