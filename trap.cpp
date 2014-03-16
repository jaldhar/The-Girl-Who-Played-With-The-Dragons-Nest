#include "trap.h"

struct Trap::TrapImpl {
    TrapImpl();
    ~TrapImpl()=default;

    bool    _sprung;
};

Trap::Trap() : Item("a", "trap", ITEMTYPE::TRAP), _impl { new Trap::TrapImpl() } {
}

Trap::~Trap() {

}

bool Trap::sprung() const {
    return _impl->_sprung;
}

void Trap::setSprung(bool spring) {
    _impl->_sprung = spring;
}

Trap::TrapImpl::TrapImpl() {
    _sprung = false;
}
