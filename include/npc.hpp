#ifndef NPC_HPP
#define NPC_HPP

#include <iostream>
#include <memory>
#include <mutex>
#include <vector>
#include <string>

// Forward declarations
class IFightObserver;
class Knight;
class Dragon;
class Pegasus;

enum class NPCType { KNIGHT, DRAGON, PEGASUS };

class NPC : public std::enable_shared_from_this<NPC> {
protected:
    NPCType type;
    std::string name;
    double x, y;
    bool alive;
    mutable std::mutex mtx;
    std::vector<std::shared_ptr<IFightObserver>> observers;

public:
    NPC(NPCType t, const std::string& name, double x, double y);
    virtual ~NPC() = default;
    
    NPCType getType() const { return type; }
    std::string getName() const { return name; }
    double getX() const { return x; }
    double getY() const { return y; }
    bool isAlive() const { return alive; }
    
    void setPosition(double newX, double newY);
    double distanceTo(const NPC* other) const;
    bool isClose(const NPC* other, double distance) const;
    void kill() { alive = false; }
    
    virtual void move();
    virtual void print() const = 0;
    virtual char getSymbol() const = 0;
    
    // Visitor pattern
    virtual bool accept(std::shared_ptr<NPC> attacker) = 0;
    virtual bool fight(std::shared_ptr<NPC> other) = 0;
    
    // Visitor методы для конкретных типов
    virtual bool visit(std::shared_ptr<Knight> other) = 0;
    virtual bool visit(std::shared_ptr<Dragon> other) = 0;
    virtual bool visit(std::shared_ptr<Pegasus> other) = 0;
    
    // Наблюдатели
    void subscribe(std::shared_ptr<IFightObserver> observer);
    void fight_notify(std::shared_ptr<NPC> defender, bool win);
    
    static int rollDice();
};

class IFightObserver {
public:
    virtual ~IFightObserver() = default;
    virtual void on_fight(std::shared_ptr<NPC> attacker, 
                         std::shared_ptr<NPC> defender, 
                         bool win) = 0;
};

#endif