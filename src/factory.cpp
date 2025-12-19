#include "factory.hpp"
#include "knight.hpp"    // ← ДОБАВИТЬ
#include "dragon.hpp"    // ← ДОБАВИТЬ  
#include "pegasus.hpp"   // ← ДОБАВИТЬ
#include "observer.hpp"
#include <memory>

std::shared_ptr<NPC> Factory::create(NPCType type, 
                                    const std::string& name, 
                                    double x, double y) {
    std::shared_ptr<NPC> npc;
    
    switch (type) {
        case NPCType::KNIGHT:
            npc = std::make_shared<Knight>(name, x, y);
            break;
        case NPCType::DRAGON:
            npc = std::make_shared<Dragon>(name, x, y);
            break;
        case NPCType::PEGASUS:
            npc = std::make_shared<Pegasus>(name, x, y);
            break;
    }
    
    if (npc) {
        // Подписываем на наблюдателей
        npc->subscribe(ConsoleObserver::get());
        npc->subscribe(FileObserver::get());
    }
    
    return npc;
}