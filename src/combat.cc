#include <cstdlib>
#include "combat.h"

struct Combat::CombatImpl {
    CombatImpl();
    CombatImpl(int health, int offense, int defense);
    ~CombatImpl()=default;

    int         health_;
    int         offense_;
    int         defense_;
};

Combat::Combat() : impl_ { new Combat::CombatImpl() } {

}

Combat::Combat(int health, int offense, int defense) :
    impl_ { new Combat::CombatImpl(health, offense, defense) } {

}

Combat::~Combat() {

}

int Combat::attack() {
    return rand() % 6 + rand() % 6 + impl_->offense_;
}

int Combat::defend() {
    return rand() % 6 + rand() % 6 + impl_->defense_;
}

int Combat::defense() const {
    return impl_->defense_;
}
void Combat::setDefense(int defense) {
    impl_->defense_ += defense;
}

int Combat::health() const {
    return impl_->health_;
}

void Combat::setHealth(int health) {
    impl_->health_ += health;
}

int Combat::offense() const {
    return impl_->offense_;
}

void Combat::setOffense(int offense) {
    impl_->offense_ += offense;
}

Combat::CombatImpl::CombatImpl() : CombatImpl(0, 0, 0) {
}

Combat::CombatImpl::CombatImpl(int health, int offense, int defense) :
health_{health}, offense_{offense}, defense_{defense} {
}
