#include <array>
using namespace std;

#include "player.h"

struct Player::PlayerImpl {
    PlayerImpl();
    ~PlayerImpl()=default;

    int                _facingX;
    int                _facingY;
    bool               _keepFighting;
    bool               _keepMoving;
    bool               _pickup;
    array<ITEMPTR, 4> _carried;
    array<ITEMPTR, 2>  _wielded;
} Player::_impl;

Player::Player() : Combat(10, 0, 0) {
}

int Player::facingX() const {
    return _impl._facingX;
}

void Player::setFacingX(int  x) {
    _impl._facingX = x;
}

int Player::facingY() const {
    return _impl._facingY;
}

void Player::setFacingY(int  y) {
    _impl._facingY = y;
}

bool Player::keepFighting() const {
    return _impl._keepFighting;
}

void Player::setKeepFighting(bool fight) {
    _impl._keepFighting = fight;
}

bool Player::keepMoving() const {
    return _impl._keepMoving;
}

void Player::setKeepMoving(bool move) {
    _impl._keepMoving = move;
}

bool Player::pickup() const {
    return _impl._pickup;
}

void Player::setPickup(bool pickup) {
    _impl._pickup = pickup;
}

bool Player::carry(Item *item) {
    for (auto & carried : _impl._carried) {
        if (carried == nullptr) {
            carried.reset(item);
            return true;
        }
    }
    return false;
}

bool Player::wield(Item *item) {
    for (auto & wielded: _impl._wielded) {
        if (wielded == nullptr) {
            wielded.reset(item);
            return true;
        }
    }
    return false;
}

Item* Player::drop(int dropped) {
    switch(dropped) {
    case 1:
        return _impl._wielded[0].release();
    case 2:
        return _impl._wielded[1].release();
    case 3:
        return _impl._carried[0].release();
    case 4:
        return _impl._carried[1].release();
    case 5:
        return _impl._carried[2].release();
    case 6:
        return _impl._carried[3].release();
    default:
        return nullptr;
    }
}

void Player::foreach_carried(function<void(unique_ptr<Item>&)> callback) {
    for (auto & carried : _impl._carried) {
        callback(carried);
    }
}

void Player::foreach_wielded(function<void(unique_ptr<Item>&)> callback) {
    for (auto & wielded : _impl._wielded) {
        callback(wielded);
    }
}

Player::PlayerImpl::PlayerImpl() {
    _facingX = 0;
    _facingY = 0;
    _keepFighting = false;
    _keepMoving = false;
    _pickup = true;
}
