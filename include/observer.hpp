#ifndef OBSERVER_HPP
#define OBSERVER_HPP

#include "npc.hpp"
#include <fstream>
#include <memory>

class ConsoleObserver : public IFightObserver {
public:
    void on_fight(std::shared_ptr<NPC> attacker, 
                 std::shared_ptr<NPC> defender, 
                 bool win) override;
    
    static std::shared_ptr<ConsoleObserver> get();
};

class FileObserver : public IFightObserver {
private:
    std::ofstream log_file;
    
public:
    FileObserver(const std::string& filename = "battle_log.txt");  // ← конструктор с параметром
    ~FileObserver();
    
    void on_fight(std::shared_ptr<NPC> attacker, 
                 std::shared_ptr<NPC> defender, 
                 bool win) override;
    
    static std::shared_ptr<FileObserver> get();
};

#endif