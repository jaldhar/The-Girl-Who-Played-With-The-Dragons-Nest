#include "shield.h"
using namespace std;

Shield::Shield(string article, string name, ITEMTYPE type,
    int offenseBonus, int defenseBonus) :
    Item(article, name, type), Armament(offenseBonus, defenseBonus) {
}

Shield::~Shield() {

}
