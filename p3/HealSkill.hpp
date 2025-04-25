#ifndef __HEAl_SKILL__
#define __HEAL_SKILL__
#include "Skill.hpp"
class HealSkill: Skill{
public:
    int attack(Pet* self, Pet* opponent) override{
        //do nothing
        return -1;
    }
    int heal(Pet* self) override{
        return self->hp + self->attack/2;
    }
};
#endif