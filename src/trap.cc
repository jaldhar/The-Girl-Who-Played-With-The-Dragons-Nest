#include "trap.h"

struct Trap::TrapImpl {
    TrapImpl();
    ~TrapImpl()=default;

    bool    sprung_;
};

Trap::Trap() : Item("a", "trap", ITEMTYPE::TRAP), impl_ { new Trap::TrapImpl() } {
}

Trap::~Trap() {

}

bool Trap::sprung() const {
    return impl_->sprung_;
}

void Trap::setSprung(bool spring) {
    impl_->sprung_ = spring;
}

Trap::TrapImpl::TrapImpl() : sprung_{false} {
}
