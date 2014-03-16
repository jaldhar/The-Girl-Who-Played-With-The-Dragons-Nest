#ifndef TILE_H
#define TILE_H

#include <memory>
#include "terrain.h"

class Tile {
public:
    Tile();
    ~Tile();
    bool    passable() const;
    void    setPassable(bool p);
    TERRAIN terrain() const;
    void    setTerrain(TERRAIN terrain);
    bool    visible() const;
    void    setVisible(bool v);
    bool    isBlock();

private:
    struct TileImpl;
    std::unique_ptr<TileImpl> _impl;
};
#endif // TILE_H
