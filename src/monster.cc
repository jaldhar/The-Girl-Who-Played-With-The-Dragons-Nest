#include "monster.h"

Monster::Monster(std::string article, std::string name, ITEMTYPE type, int health,
    int offense, int defense) :
    Item(article, name, type), Combat(health, offense, defense) {
}

Monster::~Monster() {

}
