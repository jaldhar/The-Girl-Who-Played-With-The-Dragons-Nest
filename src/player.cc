#include <array>
#include "player.h"

struct Player::PlayerImpl {
    PlayerImpl();
    ~PlayerImpl()=default;

    int                facingX_;
    int                facingY_;
    bool               keepFighting_;
    bool               keepMoving_;
    bool               pickup_;
    std::array<ITEMPTR, 4> carried_;
    std::array<ITEMPTR, 2>  wielded_;
} Player::impl_;

Player::Player() : Combat(10, 0, 0) {
}

int Player::facingX() const {
    return impl_.facingX_;
}

void Player::setFacingX(int  x) {
    impl_.facingX_ = x;
}

int Player::facingY() const {
    return impl_.facingY_;
}

void Player::setFacingY(int  y) {
    impl_.facingY_ = y;
}

bool Player::keepFighting() const {
    return impl_.keepFighting_;
}

void Player::setKeepFighting(bool fight) {
    impl_.keepFighting_ = fight;
}

bool Player::keepMoving() const {
    return impl_.keepMoving_;
}

void Player::setKeepMoving(bool move) {
    impl_.keepMoving_ = move;
}

bool Player::pickup() const {
    return impl_.pickup_;
}

void Player::setPickup(bool pickup) {
    impl_.pickup_ = pickup;
}

bool Player::carry(Item *item) {
    for (auto & carried : impl_.carried_) {
        if (carried == nullptr) {
            carried.reset(item);
            return true;
        }
    }
    return false;
}

bool Player::wield(Item *item) {
    for (auto & wielded: impl_.wielded_) {
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
        return impl_.wielded_[0].release();
    case 2:
        return impl_.wielded_[1].release();
    case 3:
        return impl_.carried_[0].release();
    case 4:
        return impl_.carried_[1].release();
    case 5:
        return impl_.carried_[2].release();
    case 6:
        return impl_.carried_[3].release();
    default:
        return nullptr;
    }
}

void Player::foreach_carried(std::function<void(std::unique_ptr<Item>&)>
callback) {
    for (auto & carried : impl_.carried_) {
        callback(carried);
    }
}

void Player::foreach_wielded(std::function<void(std::unique_ptr<Item>&)>
callback) {
    for (auto & wielded : impl_.wielded_) {
        callback(wielded);
    }
}

Player::PlayerImpl::PlayerImpl() : facingX_{0}, facingY_{0},
keepFighting_{false}, keepMoving_{false}, pickup_{true}, carried_{}, wielded_{}{
}
