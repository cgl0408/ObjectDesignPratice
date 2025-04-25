#ifndef __SKILL__
#define __SKILL__
class Skill{
    virtual int attack(Pet* self, Pet* opponent);
    virtual int heal(Pet* self);
};
#endif