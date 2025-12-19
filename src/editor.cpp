#include "editor.hpp"
#include "factory.hpp"
#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>

DungeonEditor::DungeonEditor(Observer* obs) : observer(obs) {}

void DungeonEditor::addNPC(NPCType type, const std::string& name, double x, double y) {
    auto npc = Factory::create(type, name, x, y);
    if (npc) {
        npcs.push_back(npc);
    }
}

void DungeonEditor::showAll() const {
    std::cout << "=== ВСЕ NPC (" << npcs.size() << ") ===" << std::endl;
    for (const auto& npc : npcs) {
        npc->print();
    }
    std::cout << "=========================" << std::endl;
}

void DungeonEditor::save(const std::string& filename) {
    // Простая реализация - сохраняем только живых NPC
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла: " << filename << std::endl;
        return;
    }
    
    int count = 0;
    for (const auto& npc : npcs) {
        if (npc->isAlive()) count++;
    }
    
    file << count << std::endl;
    for (const auto& npc : npcs) {
        if (npc->isAlive()) {
            file << static_cast<int>(npc->getType()) << " "
                 << npc->getName() << " "
                 << npc->getX() << " "
                 << npc->getY() << std::endl;
        }
    }
    
    file.close();
    std::cout << "Сохранено " << count << " NPC в файл: " << filename << std::endl;
}

void DungeonEditor::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла: " << filename << std::endl;
        return;
    }
    
    npcs.clear(); // Очищаем текущих NPC
    
    int count;
    file >> count;
    
    for (int i = 0; i < count; i++) {
        int type;
        std::string name;
        double x, y;
        
        file >> type >> name >> x >> y;
        
        NPCType npcType = static_cast<NPCType>(type);
        addNPC(npcType, name, x, y);
    }
    
    file.close();
    std::cout << "Загружено " << count << " NPC из файла: " << filename << std::endl;
}

void DungeonEditor::startBattle(double range) {
    std::cout << "=== НАЧАЛО БОЯ (радиус: " << range << ") ===" << std::endl;
    
    // Собираем живых NPC
    std::vector<std::shared_ptr<NPC>> aliveNPCs;
    for (const auto& npc : npcs) {
        if (npc->isAlive()) {
            aliveNPCs.push_back(npc);
        }
    }
    
    // Бой каждого с каждым
    for (size_t i = 0; i < aliveNPCs.size(); i++) {
        for (size_t j = i + 1; j < aliveNPCs.size(); j++) {
            if (aliveNPCs[i]->isClose(aliveNPCs[j].get(), range)) {
                // Бросаем кубики
                int attackRoll = NPC::rollDice();
                int defenseRoll = NPC::rollDice();
                
                // Проверяем может ли атакующий победить
                bool canKill = aliveNPCs[j]->accept(aliveNPCs[i]);
                
                if (canKill && attackRoll > defenseRoll) {
                    aliveNPCs[j]->kill();
                    std::cout << "[БОЙ] " << aliveNPCs[i]->getName()
                              << "(" << attackRoll << ") победил "
                              << aliveNPCs[j]->getName()
                              << "(" << defenseRoll << ")" << std::endl;
                } else {
                    std::cout << "[БОЙ] " << aliveNPCs[i]->getName()
                              << "(" << attackRoll << ") промахнулся по "
                              << aliveNPCs[j]->getName()
                              << "(" << defenseRoll << ")" << std::endl;
                }
            }
        }
    }
    
    std::cout << "=== БОЙ ЗАВЕРШЕН ===" << std::endl;
}