#include "weapon.h"

Weapon::Weapon(std::string article, std::string name, ITEMTYPE type,
int offenseBonus, int defenseBonus) : Item(article, name, type),
Armament(offenseBonus, defenseBonus) {
}

Weapon::~Weapon() {

}
