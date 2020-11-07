#ifndef WORLD_H
#define WORLD_H

#include <functional>
#include <memory>
#include "item.h"
#include "tile.h"

class World
{
public:
    World()=default;
    ~World()=default;
    void     create();
    int      height() const;
    int      width() const;
    int      playerRow() const;
    void     setPlayerRow(int row);
    int      playerCol() const;
    void     setPlayerCol(int col);
    int      startCol() const;
    void     foreach_item(int top, int left, int height, int width,
                std::function<void(int, int, std::unique_ptr<Item>&)> callback);
    Item*    itemAt(int row, int col) const;
    void     insertItem(int row, int col, Item* item);
    bool     removeItem(int row, int col, bool destroy = false);
    void     setAllVisible(bool visibility);
    void     fov();
    Tile*    tileAt(int row, int col) const;
private:
    struct WorldImpl;
    static WorldImpl impl_;
};

#endif // WORLD_H
