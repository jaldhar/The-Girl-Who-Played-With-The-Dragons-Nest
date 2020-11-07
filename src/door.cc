#include "door.h"

struct Door::DoorImpl {
    DoorImpl();
    ~DoorImpl()=default;

    bool    horizontal_;
    bool    open_;
};

Door::Door() : Item("a", "door", ITEMTYPE::DOOR),
    impl_ { new Door::DoorImpl() } {
}

Door::~Door() {

}

bool Door::horizontal() const {
    return impl_->horizontal_;
}

void Door::setHorizontal(bool horizontal) {
    impl_->horizontal_ = horizontal;
}

bool Door::open() const {
    return impl_->open_;
}

void Door::setOpen(bool open) {
    impl_->open_ = open;
}

Door::DoorImpl::DoorImpl() :horizontal_{false}, open_{false} {
}
