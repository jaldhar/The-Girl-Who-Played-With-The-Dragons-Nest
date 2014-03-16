#ifndef SHIELD_H
#define SHIELD_H

#include <string>
#include "armament.h"
#include "item.h"

class Shield : public Item, public Armament
{
public:
    Shield(std::string article, std::string name, ITEMTYPE type,
      int offensebonus, int defensebonus);
    virtual ~Shield();
};

#endif // SHIELD_H
