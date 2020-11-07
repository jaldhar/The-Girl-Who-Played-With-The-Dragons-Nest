#include "tile.h"

struct Tile::TileImpl {
    TileImpl();
    ~TileImpl()=default;
    bool    passable_;
    bool    seen_;
    TERRAIN terrain_;
    bool    visible_;
};

Tile::Tile() : impl_ { new Tile::TileImpl() } {
}

Tile::~Tile() {

}

bool Tile::passable() const {
    return impl_->passable_;
}

void Tile::setPassable(bool passable) {
     impl_->passable_ = passable;
}

bool Tile::seen() const {
    return impl_->seen_;
}

void Tile::setSeen(bool seen) {
     impl_->seen_ = seen;
}

TERRAIN Tile::terrain() const {
    return impl_->terrain_;
}

void Tile::setTerrain(TERRAIN terrain) {
    impl_->terrain_ = terrain;
}

bool Tile::visible() const {
    return impl_->visible_;
}

void Tile::setVisible(bool visible) {
     impl_->visible_ = visible;
}

bool Tile::isBlock() {
    return (
    impl_->terrain_ == TERRAIN::H_WALL  || impl_->terrain_ == TERRAIN::V_WALL ||
    impl_->terrain_ == TERRAIN::UL_WALL || impl_->terrain_ == TERRAIN::UR_WALL ||
    impl_->terrain_ == TERRAIN::LR_WALL || impl_->terrain_ == TERRAIN::LL_WALL ||
    impl_->terrain_ == TERRAIN::TT_WALL || impl_->terrain_ == TERRAIN::RT_WALL ||
    impl_->terrain_ == TERRAIN::BT_WALL || impl_->terrain_ == TERRAIN::LT_WALL ||
    impl_->terrain_ == TERRAIN::C_WALL  ||
    impl_->terrain_ == TERRAIN::H_DOOR_CLOSED || impl_->terrain_ == TERRAIN::V_DOOR_CLOSED);
}

Tile::TileImpl::TileImpl() : passable_{false}, seen_{false},
terrain_{TERRAIN::EMPTY}, visible_{false} {
}

