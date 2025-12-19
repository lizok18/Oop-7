#include "pegasus.hpp"
#include "knight.hpp"    // ← ДОБАВИТЬ
#include "dragon.hpp"    // ← ДОБАВИТЬ
#include <iostream>

Pegasus::Pegasus(const std::string& name, double x, double y)
    : NPC(NPCType::PEGASUS, name, x, y) {}

void Pegasus::print() const {
    std::cout << "Пегас '" << name << "' (" << x << ", " << y << ") "
              << (alive ? "жив" : "мертв") << std::endl;
}

bool Pegasus::accept(std::shared_ptr<NPC> attacker) {
    return attacker->fight(shared_from_this());
}

bool Pegasus::fight(std::shared_ptr<NPC> other) {
    return other->visit(std::dynamic_pointer_cast<Pegasus>(shared_from_this()));
}

bool Pegasus::visit(std::shared_ptr<Knight> other) {
    fight_notify(other, true);
    return true;
}

bool Pegasus::visit(std::shared_ptr<Dragon> other) {
    fight_notify(other, false);
    return false;
}

bool Pegasus::visit(std::shared_ptr<Pegasus> other) {
    fight_notify(other, false);
    return false;
}