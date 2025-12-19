#ifndef FACTORY_HPP
#define FACTORY_HPP

#include "npc.hpp"
#include <memory>

class Factory {
public:
    static std::shared_ptr<NPC> create(NPCType type, 
                                      const std::string& name, 
                                      double x, double y);
};

#endif