#include <cstdlib>
using namespace std;

#include "combat.h"

struct Combat::CombatImpl {
    CombatImpl();
    CombatImpl(int health, int offense, int defense);
    ~CombatImpl()=default;

    int         _health;
    int         _offense;
    int         _defense;
};

Combat::Combat() : _impl { new Combat::CombatImpl() } {

}

Combat::Combat(int health, int offense, int defense) :
    _impl { new Combat::CombatImpl(health, offense, defense) } {

}

Combat::~Combat() {

}

int Combat::attack() {
    return rand() % 6 + rand() % 6 + _impl->_offense;
}

int Combat::defend() {
    return rand() % 6 + rand() % 6 + _impl->_defense;
}

int Combat::defense() const {
    return _impl->_defense;
}
void Combat::setDefense(int defense) {
    _impl->_defense += defense;
}

int Combat::health() const {
    return _impl->_health;
}

void Combat::setHealth(int health) {
    _impl->_health += health;
}

int Combat::offense() const {
    return _impl->_offense;
}

void Combat::setOffense(int offense) {
    _impl->_offense += offense;
}

Combat::CombatImpl::CombatImpl() {
    CombatImpl(0, 0, 0);
}

Combat::CombatImpl::CombatImpl(int health, int offense, int defense) {
    _health  = health;
    _offense = offense;
    _defense = defense;
}
