#include "armament.h"

struct Armament::ArmamentImpl {
    ArmamentImpl();
    ArmamentImpl(int defenseBonus, int offenseBonus);
    ~ArmamentImpl()=default;

    int         defenseBonus_;
    int         offenseBonus_;
};

Armament::Armament() : impl_ { new Armament::ArmamentImpl() } {
}

Armament::Armament(int defenseBonus, int offenseBonus) :
    impl_ { new Armament::ArmamentImpl(defenseBonus, offenseBonus) } {
}

Armament::~Armament() {

}


int Armament::defenseBonus() const {
    return impl_->defenseBonus_;
}

void Armament::setDefenseBonus(int defenseBonus) {
    impl_->defenseBonus_ += defenseBonus;
}

int Armament::offenseBonus() const {
    return impl_->offenseBonus_;
}

void Armament::setOffenseBonus(int offenseBonus) {
    impl_->offenseBonus_ += offenseBonus;
}

Armament::ArmamentImpl::ArmamentImpl() : ArmamentImpl(0, 0) {
}

Armament::ArmamentImpl::ArmamentImpl(int defenseBonus, int offenseBonus) :
defenseBonus_{defenseBonus}, offenseBonus_{offenseBonus} {
}
