#ifndef TILE_H
#define TILE_H

#include <memory>
#include "terrain.h"

class Tile {
public:
    Tile();
    ~Tile();
    bool    passable() const;
    void    setPassable(bool passable);
    bool    seen() const;
    void    setSeen(bool seen);
    TERRAIN terrain() const;
    void    setTerrain(TERRAIN terrain);
    bool    visible() const;
    void    setVisible(bool visible);
    bool    isBlock();

private:
    struct TileImpl;
    std::unique_ptr<TileImpl> impl_;
};
#endif // TILE_H
