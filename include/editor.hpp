#ifndef EDITOR_HPP
#define EDITOR_HPP

#include <vector>
#include <memory>
#include <string>
#include "npc.hpp"
#include "observer.hpp"

// Forward declaration вместо инклуда
class Observer;

class DungeonEditor {
private:
    std::vector<std::shared_ptr<NPC>> npcs;
    Observer* observer;

public:
    DungeonEditor(Observer* obs = nullptr); 
    
    // Добавить NPC
    void addNPC(NPCType type, const std::string& name, double x, double y);
    
    // Показать всех NPC
    void showAll() const;
    
    // Сохранить в файл
    void save(const std::string& filename);
    
    // Загрузить из файла
    void load(const std::string& filename);
    
    // Запустить бой
    void startBattle(double range);
    
    // Получить список NPC
    std::vector<std::shared_ptr<NPC>>& getNPCs() { return npcs; }
    const std::vector<std::shared_ptr<NPC>>& getNPCs() const { return npcs; }
};

#endif