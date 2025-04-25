#ifndef __PET__
#define __PET__

#include <string>
#include <memory>
#include "AttackSkill.hpp"
#include "HealSkill.hpp"

class Pet{
public:
    std::string name;
    int hp;
    int attack;
    std::unique_ptr<HealSkill> healSkill;
    std::unique_ptr<AttackSkill> attackSkill;
    bool attakTarget(Pet* opponent);
    bool healSelf();
    Pet(std::string name, int hp, int attack);
    
};
#endif