#ifndef PEGASUS_HPP
#define PEGASUS_HPP

#include "npc.hpp"
#include <memory>

// Forward declaration
class Knight;
class Dragon;

class Pegasus : public NPC {
public:
    Pegasus(const std::string& name, double x, double y);
    
    void print() const override;
    char getSymbol() const override { return 'P'; }
    
    bool accept(std::shared_ptr<NPC> attacker) override;
    bool fight(std::shared_ptr<NPC> other) override;
    
    // Visitor методы
    bool visit(std::shared_ptr<Knight> other) override;
    bool visit(std::shared_ptr<Dragon> other) override;
    bool visit(std::shared_ptr<Pegasus> other) override;
};

#endif