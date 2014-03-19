#include "tile.h"

struct Tile::TileImpl {
    TileImpl();
    ~TileImpl()=default;
    bool    _passable;
    bool    _seen;
    TERRAIN _terrain;
    bool    _visible;
};

Tile::Tile() : _impl { new Tile::TileImpl() } {
}

Tile::~Tile() {

}

bool Tile::passable() const {
    return _impl->_passable;
}

void Tile::setPassable(bool passable) {
     _impl->_passable = passable;
}

bool Tile::seen() const {
    return _impl->_seen;
}

void Tile::setSeen(bool seen) {
     _impl->_seen = seen;
}

TERRAIN Tile::terrain() const {
    return _impl->_terrain;
}

void Tile::setTerrain(TERRAIN terrain) {
    _impl->_terrain = terrain;
}

bool Tile::visible() const {
    return _impl->_visible;
}

void Tile::setVisible(bool visible) {
     _impl->_visible = visible;
}

bool Tile::isBlock() {
    return (
    _impl->_terrain == TERRAIN::H_WALL  || _impl->_terrain == TERRAIN::V_WALL ||
    _impl->_terrain == TERRAIN::UL_WALL || _impl->_terrain == TERRAIN::UR_WALL ||
    _impl->_terrain == TERRAIN::LR_WALL || _impl->_terrain == TERRAIN::LL_WALL ||
    _impl->_terrain == TERRAIN::TT_WALL || _impl->_terrain == TERRAIN::RT_WALL ||
    _impl->_terrain == TERRAIN::BT_WALL || _impl->_terrain == TERRAIN::LT_WALL ||
    _impl->_terrain == TERRAIN::C_WALL  ||
    _impl->_terrain == TERRAIN::H_DOOR_CLOSED || _impl->_terrain == TERRAIN::V_DOOR_CLOSED);
}

Tile::TileImpl::TileImpl() {
    _passable = false;
    _seen = false;
    _terrain = TERRAIN::EMPTY;
    _visible = false;
}

