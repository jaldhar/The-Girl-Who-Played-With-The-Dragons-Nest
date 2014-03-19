#include <algorithm>
#include <array>
#include <bitset>
#include <cmath>
#include <cstdlib>
#include <map>
#include <vector>
#include <utility>
using namespace std;

#include "door.h"
#include "key.h"
#include "monster.h"
#include "potion.h"
#include "shield.h"
#include "trap.h"
#include "weapon.h"
#include "world.h"

static const int MAP_HEIGHT       = 15;
static const int MAP_WIDTH        = 15;

typedef unique_ptr<Tile>    TILEPTR;

struct World::WorldImpl {
    WorldImpl()=default;
    ~WorldImpl()=default;
    void generateMaze();
    void makeFloor(int row, int col);
    void addItem(int row, int col);
    void addDoors();
    void addExits();
    void addWalls();
    void specializeWalls();

    array<array<TILEPTR, MAP_WIDTH>,MAP_HEIGHT> _map;
    int                                         _playerRow;
    int                                         _playerCol;
    int                                         _startCol;
    int                                         _endCol;
    map<pair<int, int>, ITEMPTR>                _items;
} World::_impl;

void World::create() {
    // Begin by filling in the entire grid.
    for (auto & row : _impl._map) {
        for (auto & col : row) {
            col = TILEPTR(new Tile());
        }
    }

    // Build the maze (including items, monsters and traps,)
    _impl.generateMaze();

    // Add exits and set the player position.
    _impl.addExits();

    // Add basic walls
    _impl.addWalls();

    // Doors have to be placed separately after walls.
    _impl.addDoors();

    // Make walls fancier.
    _impl.specializeWalls();
}

int World::height() const {
    return MAP_HEIGHT;
}

int World::width() const {
    return MAP_WIDTH;
}

int World::playerRow() const {
    return _impl._playerRow;
}

void World::setPlayerRow(int row) {
    _impl._playerRow = row;
}

int World::playerCol() const {
    return _impl._playerCol;
}

void World::setPlayerCol(int col) {
    _impl._playerCol = col;
}

int World::startCol() const {
    return _impl._startCol;
}

void  World::foreach_item(int top, int left, int height, int width,
    function<void(int, int, ITEMPTR&)> callback) {
    for(auto & i : _impl._items) {
        int row = i.first.first;
        int col = i.first.second;
        if (row < top || row > top + height - 1 || col < left ||
        col > left + width - 1) {
            continue;
        }
        callback(row, col, i.second);
    }
}

Item* World::itemAt(int row, int col) const {
    auto item = _impl._items.find(make_pair(row, col));
    if (item == _impl._items.end()) {
        return nullptr;
    }

    return item->second.get();
}

void World::insertItem(int row, int col, Item* item) {
    _impl._items[make_pair(row, col)] = ITEMPTR(item);
}

bool World::removeItem(int row, int col, bool destroy) {
    auto item = _impl._items.find(make_pair(row, col));

    if (item == _impl._items.end()) {
        return false;
    }

    if (destroy) {
        delete item->second.release();
    } else {
        item->second.release();
    }
    _impl._items.erase(item);

    return true;
}

void World::setAllVisible(bool visibility) {
    for (auto & row : _impl._map) {
        for (auto & col : row) {
            col->setVisible(visibility);
        }
    }
}

void World::fov() {
    setAllVisible(false);

    for (int i = _impl._playerRow - 1; i < _impl._playerRow + 2; i++) {
        if (i < 0 || i >= MAP_HEIGHT) {
            continue;
        }
        for (int j = _impl._playerCol - 1; j < _impl._playerCol + 2; j++) {
            if (j < 0 || j >= MAP_WIDTH) {
                continue;
            }
            _impl._map[i][j]->setVisible(true);
            _impl._map[i][j]->setSeen(true);
        }
    }
}

Tile* World::tileAt(int row, int col) const {
    return _impl._map[row][col].get();
}

// private methods

void World::WorldImpl::generateMaze() {
    // Build maze (Algorithm based on VB/JS examples at
    // http://www.roguebasin.com/index.php?title=Simple_maze)
    int done = 0;
    vector<pair<int, int>> dirs;
    dirs.push_back(make_pair(-1,0));
    dirs.push_back(make_pair(1,0));
    dirs.push_back(make_pair(0,-1));
    dirs.push_back(make_pair(0,1));

    do {
        // this code is used to make sure the numbers are odd
        int row = 1 + rand() % ((MAP_HEIGHT - 1) / 2) * 2;
        int col = 1 + rand() % ((MAP_WIDTH - 1) / 2) * 2;

        // Start tile.
        if (done == 0) {
            makeFloor(row, col);
        }

        if (_impl._map[row][col]->terrain() == TERRAIN::FLOOR) {
            //Randomize Directions
            random_shuffle(dirs.begin(), dirs.end());

            bool blocked = true;

            do {
                if (rand() % 5 == 0) {
                    random_shuffle(dirs.begin(), dirs.end());
                }

                blocked = true;
                for (int i = 0; i < 4; i++) {
                    // Determine which direction the tile is
                    int r = row + dirs[i].first * 2;
                    int c = col + dirs[i].second * 2;
                    //Check to see if the tile can be used
                    if (r >= 1 && r < MAP_HEIGHT - 1 && c >= 1 && c < MAP_WIDTH - 1) {
                        if (_impl._map[r][c]->terrain() != TERRAIN::FLOOR) {
                            //create destination location
                            makeFloor(r, c);
                            //create intermediate location
                            makeFloor(row + dirs[i].first, col + dirs[i].second);
                            row = r;
                            col = c;
                            blocked = false;
                            done++;
                            break;
                        }
                    }
                }
                //recursive, no directions found, loop back a node
            } while (!blocked);
        }
    } while (done + 1 < ((MAP_HEIGHT - 1) * (MAP_WIDTH - 1)) / 4);
}


void World::WorldImpl::makeFloor(int row, int col) {
    _impl._map[row][col]->setTerrain(TERRAIN::FLOOR);
    _impl._map[row][col]->setPassable(true);
    addItem(row, col);
}

void World::WorldImpl::addItem(int row, int col) {

    // Start space always empty
    if (row == 0 && col == _startCol) {
        return;
    // End space always dragon
    } else if (row == MAP_HEIGHT - 1 && col == _endCol) {
        Monster* dragon = new Monster("the", "dragon", ITEMTYPE::DRAGON, 1, 6, 6);
        _items[make_pair(row, col)] = ITEMPTR(dragon);
    } else {
        int r = rand() % 100;

        // empty
        if (r < 50) {
            return;

        // monster
        } else if (r < 75) {
            Monster* monster;
            int rr = rand() % 10;
            if (row < MAP_HEIGHT / 3) {
                if (rr < 4) {
                    monster = new Monster("a", "vampire bat", ITEMTYPE::BAT, 1, 0, 2);
                } else if (rr < 8) {
                    monster = new Monster("a", "giant rat", ITEMTYPE::RAT, 1, 1, 1);
                } else if (rr < 9) {
                    monster = new Monster("a", "zombie", ITEMTYPE::ZOMBIE, 1, 1, 1);
                } else {
                    monster = new Monster("a", "kobold", ITEMTYPE::KOBOLD, 1, 1, 2);
                }
            } else if (row < MAP_HEIGHT * 2 / 3) {
                if (rr < 4) {
                    monster = new Monster("a", "hobgoblin", ITEMTYPE::HOBGOBLIN, 1, 1, 2);
                } else if (rr < 8) {
                    monster = new Monster("an", "orc", ITEMTYPE::ORC, 1, 2, 2);
                } else if (rr < 9) {
                    monster = new Monster("a", "giant spider", ITEMTYPE::SPIDER, 1, 3, 2);
                } else {
                    monster = new Monster("a", "gelatinous cube", ITEMTYPE::CUBE, 1, 1, 5);
                }
            } else {
                if (rr < 2) {
                    monster = new Monster("a", "lizard man", ITEMTYPE::LIZARDMAN, 1, 2, 4);
                } else if (rr < 4) {
                    monster = new Monster("a", "naga", ITEMTYPE::NAGA, 1, 3, 3);
                } else if (rr < 6) {
                    monster = new Monster("a", "troll", ITEMTYPE::TROLL, 1, 4, 2);
                } else if (rr < 8) {
                    monster = new Monster("a", "minotaur", ITEMTYPE::MINOTAUR, 1, 5, 3);
                } else if (rr < 9) {
                    monster = new Monster("a", "wizard", ITEMTYPE::WIZARD, 1, 5, 5);
                } else {
                    monster = new Monster("a", "floating eye", ITEMTYPE::FLOATINGEYE, 1, 5, 5);
                }
            }
            _items[make_pair(row, col)] = ITEMPTR(monster);

        // item
        } else if (r < 90) {
            int r = rand() % 100;
            if (r < 40) {
                _items[make_pair(row, col)] = ITEMPTR(new Potion());
            } else if (r < 60) {
                _items[make_pair(row, col)] = ITEMPTR(new Key());
            } else if (r < 70) {
                _items[make_pair(row, col)] = ITEMPTR(new Shield("a", "buckler", ITEMTYPE::SHIELD, 0, 1));
            } else if (r < 80) {
                _items[make_pair(row, col)] = ITEMPTR(new Shield("a", "shield", ITEMTYPE::SHIELD, 0, 2));
            } else if (r < 90) {
                _items[make_pair(row, col)] = ITEMPTR(new Weapon("a", "sword", ITEMTYPE::WEAPON, 0, 1));
            } else {
                _items[make_pair(row, col)] = ITEMPTR(new Weapon("a", "battleaxe", ITEMTYPE::WEAPON, 0, 2));
            }
            return;

        // trap
        } else {
            _items[make_pair(row, col)] = ITEMPTR(new Trap());
        }
    }
}

void World::WorldImpl::addDoors() {
    World world;

    for (int row = 1; row < MAP_HEIGHT - 1; row++) {
        for (int col = 1; col < MAP_WIDTH - 1; col++) {
            if (_map[row][col]->terrain() != TERRAIN::FLOOR) {
                continue;
            }
            int r = rand() % 100;
            if (r < 30) {
                // Check how many walls are adjacent to the door
                int adjacent = 0;
                if (_map[row - 1][col]->terrain() == TERRAIN::C_WALL) {
                    adjacent++;
                }
                if (_map[row + 1][col]->terrain() == TERRAIN::C_WALL) {
                    adjacent++;
                }
                if (_map[row][col - 1]->terrain() == TERRAIN::C_WALL) {
                    adjacent += 3;
                }
                if (_map[row][col + 1]->terrain() == TERRAIN::C_WALL) {
                    adjacent += 3;
                }

                // Unless there are exactly 2 walls horizontally or
                // vertically, no door.
                if (adjacent != 2 && adjacent != 6) {
                    continue;
                }

                // Now check if any doors already exist next to this door
                // if so, no door.
                if (adjacent == 2) {
                    if (dynamic_cast<Door*>(world.itemAt(row, col - 1)) || dynamic_cast<Door*>(world.itemAt(row, col + 1))) {
                        continue;
                     }
                }

                if (adjacent == 6) {
                    if (dynamic_cast<Door*>(world.itemAt(row - 1, col)) || dynamic_cast<Door*>(world.itemAt(row + 1, col))) {
                        continue;
                     }
                }

                Door* door = new Door();
                if (adjacent == 6) {
                    door->setHorizontal(true);
                }
                _items[make_pair(row, col)] = ITEMPTR(door);
            }
        }
    }
}

void World::WorldImpl::addExits() {
    vector<int> freeCols;
    for (int i = 1; i < MAP_WIDTH - 1; i++) {
        if (_impl._map[1][i]->terrain() == TERRAIN::FLOOR) {
            freeCols.push_back(i);
        }
    }
    _startCol = freeCols[rand() % freeCols.size()];
    makeFloor(0, _startCol);

    _playerRow = 0;
    _playerCol = _startCol;

    freeCols.clear();
    for (int i = 1; i < MAP_WIDTH - 1; i++) {
        if (_impl._map[MAP_HEIGHT - 2][i]->terrain() == TERRAIN::FLOOR) {
            freeCols.push_back(i);
        }
    }
    _endCol = freeCols[rand() % freeCols.size()];
    makeFloor(MAP_HEIGHT - 1, _endCol);
}

void World::WorldImpl::addWalls() {
    //First pass puts a center wall adjacent to any floor or corridor.
    for (int row = 0; row < MAP_HEIGHT; row++) {
        for (int col = 0; col < MAP_WIDTH; col++) {

            auto & t = _map[row][col];

            if (t->terrain() != TERRAIN::EMPTY) {
                continue;
            }

            for (int x = row - 1; x < row + 2; x++) {

                if (x < 0 || x >= MAP_HEIGHT) {
                    continue;
                }

                for (int y = col - 1; y < col + 2; y++) {

                    if (y < 0 || y >= MAP_WIDTH) {
                        continue;
                    }

                    if (x == row && y == col) {
                        continue;
                    }

                    TERRAIN c = _map[x][y]->terrain();
                    if (c == TERRAIN::FLOOR) {
                        t->setTerrain(TERRAIN::C_WALL);
                        t->setPassable(false);
                        goto end;
                    }
                }
            }

            end: ;
        }
    }
}

void World::WorldImpl::specializeWalls() {

    // Second pass makes specific wall types as needed.
    map<string, TERRAIN> walls = {
        {"01011010", TERRAIN::C_WALL},
        {"01011000", TERRAIN::BT_WALL},
        {"01010010", TERRAIN::RT_WALL},
        {"01010000", TERRAIN::LR_WALL},
        {"01001010", TERRAIN::LT_WALL},
        {"01001000", TERRAIN::LL_WALL},
        {"01000010", TERRAIN::V_WALL},
        {"01000000", TERRAIN::V_WALL},
        {"00011010", TERRAIN::TT_WALL},
        {"00011000", TERRAIN::H_WALL},
        {"00010000", TERRAIN::H_WALL},
        {"00010010", TERRAIN::UR_WALL},
        {"00001010", TERRAIN::UL_WALL},
        {"00001000", TERRAIN::H_WALL},
        {"00000010", TERRAIN::V_WALL},
        {"00000000", TERRAIN::C_WALL},
    };

    for (int row = 0; row < MAP_HEIGHT; row++) {
        for (int col = 0; col < MAP_WIDTH; col++) {

            auto & t = _map[row][col];

            if (t->terrain() != TERRAIN::C_WALL) {
                continue;
            }

            int count = 7;
            bitset<8> edgeset; // represent the edges as a binary number
            for (int y = row - 1; y < row + 2; y++) {

                if (y < 0 || y > MAP_HEIGHT - 1) {
                    count -= 3;
                    continue;
                }

                for (int x = col - 1; x < col + 2; x++) {
                    if (x < 0 || x > MAP_WIDTH - 1) {
                        count--;
                        continue;
                    }

                    if (y == row && x == col) {
                        continue;
                    }

                    if (_map[y][x]->isBlock()) {
                        edgeset.set(count);
                    }

                    auto item = _items.find(make_pair(y, x));
                    if (item != _items.end() &&
                    dynamic_cast<Door*>(item->second.get())) {
                        edgeset.set(count);
                    }

                    count--;
                }
            }

            for (auto& wall: walls) {
                bitset<8> mask(wall.first);
                if ((edgeset & mask) == mask) {
                    t->setTerrain(wall.second);
                }
            }
        }
    }
}
