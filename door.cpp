#include "door.h"

struct Door::DoorImpl {
    DoorImpl();
    ~DoorImpl()=default;

    bool    _horizontal;
    bool    _open;
};

Door::Door() : Item("a", "door", ITEMTYPE::DOOR),
    _impl { new Door::DoorImpl() } {
}

Door::~Door() {

}

bool Door::horizontal() const {
    return _impl->_horizontal;
}

void Door::setHorizontal(bool horizontal) {
    _impl->_horizontal = horizontal;
}

bool Door::open() const {
    return _impl->_open;
}

void Door::setOpen(bool open) {
    _impl->_open = open;
}

Door::DoorImpl::DoorImpl() {
    _open = false;
    _horizontal = false;
}
