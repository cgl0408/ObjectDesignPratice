
#ifndef __ATTACK_SKILL__
#define __ATTACK_SKILL__
#include "Skill.hpp"
class AttackSkill: Skill{
public:
    int attack(Pet* self, Pet* opponent) override{
        return opponent->hp - self->attack;
    }
    int heal(Pet* self) override{
        //do nothing
        return -1;
    }
};
#endif