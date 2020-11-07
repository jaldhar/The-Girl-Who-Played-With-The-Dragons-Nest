#ifndef MONSTER_H
#define MONSTER_H

#include <string>
#include "combat.h"
#include "item.h"

class Monster: public Item, public Combat {
public:
    Monster(std::string article, std::string name, ITEMTYPE type, int health,
    int offense, int defense);
    virtual ~Monster();
};

#endif // MONSTER_H
