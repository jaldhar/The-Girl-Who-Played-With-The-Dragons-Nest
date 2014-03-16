#include "weapon.h"
using namespace std;

Weapon::Weapon(string article, string name, ITEMTYPE type, int offenseBonus, int defenseBonus) :
    Item(article, name, type), Armament(offenseBonus, defenseBonus) {
}

Weapon::~Weapon() {

}
