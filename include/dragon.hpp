#ifndef DRAGON_HPP
#define DRAGON_HPP

#include "npc.hpp"
#include <memory>

// Forward declaration
class Knight;
class Pegasus;

class Dragon : public NPC {
public:
    Dragon(const std::string& name, double x, double y);
    
    void print() const override;
    char getSymbol() const override { return 'D'; }
    
    bool accept(std::shared_ptr<NPC> attacker) override;
    bool fight(std::shared_ptr<NPC> other) override;
    
    // Visitor методы
    bool visit(std::shared_ptr<Knight> other) override;
    bool visit(std::shared_ptr<Dragon> other) override;
    bool visit(std::shared_ptr<Pegasus> other) override;
};

#endif