#include "monster.h"

using namespace std;

Monster::Monster(string article, string name, ITEMTYPE type, int health,
    int offense, int defense) :
    Item(article, name, type), Combat(health, offense, defense) {
}

Monster::~Monster() {

}
