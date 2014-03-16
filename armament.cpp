#include "armament.h"

using namespace std;

struct Armament::ArmamentImpl {
    ArmamentImpl();
    ArmamentImpl(int defenseBonus, int offenseBonus);
    ~ArmamentImpl()=default;

    int         _defenseBonus;
    int         _offenseBonus;
};

Armament::Armament() : _impl { new Armament::ArmamentImpl() } {
}

Armament::Armament(int defenseBonus, int offenseBonus) :
    _impl { new Armament::ArmamentImpl(defenseBonus, offenseBonus) } {
}

Armament::~Armament() {

}


int Armament::defenseBonus() const {
    return _impl->_defenseBonus;
}

void Armament::setDefenseBonus(int defenseBonus) {
    _impl->_defenseBonus += defenseBonus;
}

int Armament::offenseBonus() const {
    return _impl->_offenseBonus;
}

void Armament::setOffenseBonus(int offenseBonus) {
    _impl->_offenseBonus += offenseBonus;
}

Armament::ArmamentImpl::ArmamentImpl() {
    ArmamentImpl(0, 0);
}

Armament::ArmamentImpl::ArmamentImpl(int defenseBonus, int offenseBonus) {
    _defenseBonus = defenseBonus;
    _offenseBonus = offenseBonus;
}
