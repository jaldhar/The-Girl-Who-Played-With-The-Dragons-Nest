#ifndef WEAPON_H
#define WEAPON_H

#include <string>
#include "armament.h"
#include "item.h"

class Weapon : public Item, public Armament {
public:
    Weapon(std::string article, std::string name, ITEMTYPE type,
      int offensebonus, int defensebonus);
    virtual ~Weapon();
};

#endif // WEAPON_H
