#include <algorithm>
#include <array>
#include <bitset>
#include <cmath>
#include <cstdlib>
#include <map>
#include <vector>
#include <utility>
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

typedef std::unique_ptr<Tile>    TILEPTR;

struct World::WorldImpl {
    WorldImpl();
    ~WorldImpl()=default;
    void generateMaze();
    void makeFloor(int row, int col);
    void addItem(int row, int col);
    void addDoors();
    void addExits();
    void addWalls();
    void specializeWalls();

    std::array<std::array<TILEPTR, MAP_WIDTH>,MAP_HEIGHT> map_;
    int                                         playerRow_;
    int                                         playerCol_;
    int                                         startCol_;
    int                                         endCol_;
    std::map<std::pair<int, int>, ITEMPTR>      items_;
} World::impl_;

void World::create() {
    // Begin by filling in the entire grid.
    for (auto & row : impl_.map_) {
        for (auto & col : row) {
            col = TILEPTR(new Tile());
        }
    }

    // Build the maze (including items, monsters and traps,)
    impl_.generateMaze();

    // Add exits and set the player position.
    impl_.addExits();

    // Add basic walls
    impl_.addWalls();

    // Doors have to be placed separately after walls.
    impl_.addDoors();

    // Make walls fancier.
    impl_.specializeWalls();
}

int World::height() const {
    return MAP_HEIGHT;
}

int World::width() const {
    return MAP_WIDTH;
}

int World::playerRow() const {
    return impl_.playerRow_;
}

void World::setPlayerRow(int row) {
    impl_.playerRow_ = row;
}

int World::playerCol() const {
    return impl_.playerCol_;
}

void World::setPlayerCol(int col) {
    impl_.playerCol_ = col;
}

int World::startCol() const {
    return impl_.startCol_;
}

void  World::foreach_item(int top, int left, int height, int width,
    std::function<void(int, int, ITEMPTR&)> callback) {
    for(auto & i : impl_.items_) {
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
    auto item = impl_.items_.find(std::make_pair(row, col));
    if (item == impl_.items_.end()) {
        return nullptr;
    }

    return item->second.get();
}

void World::insertItem(int row, int col, Item* item) {
    impl_.items_[std::make_pair(row, col)] = ITEMPTR(item);
}

bool World::removeItem(int row, int col, bool destroy) {
    auto item = impl_.items_.find(std::make_pair(row, col));

    if (item == impl_.items_.end()) {
        return false;
    }

    if (destroy) {
        delete item->second.release();
    } else {
        item->second.release();
    }
    impl_.items_.erase(item);

    return true;
}

void World::setAllVisible(bool visibility) {
    for (auto & row : impl_.map_) {
        for (auto & col : row) {
            col->setVisible(visibility);
        }
    }
}

void World::fov() {
    setAllVisible(false);

    for (int i = impl_.playerRow_ - 1; i < impl_.playerRow_ + 2; i++) {
        if (i < 0 || i >= MAP_HEIGHT) {
            continue;
        }
        for (int j = impl_.playerCol_ - 1; j < impl_.playerCol_ + 2; j++) {
            if (j < 0 || j >= MAP_WIDTH) {
                continue;
            }
            impl_.map_[i][j]->setVisible(true);
            impl_.map_[i][j]->setSeen(true);
        }
    }
}

Tile* World::tileAt(int row, int col) const {
    return impl_.map_[row][col].get();
}

// private methods

World::WorldImpl::WorldImpl() : map_{}, playerRow_{0}, playerCol_{0},
startCol_{0}, endCol_{0}, items_{} {
}

void World::WorldImpl::generateMaze() {
    // Build maze (Algorithm based on VB/JS examples at
    // http://www.roguebasin.com/index.php?title=Simple_maze)
    int done = 0;
    std::vector<std::pair<int, int>> dirs;
    dirs.push_back(std::make_pair(-1,0));
    dirs.push_back(std::make_pair(1,0));
    dirs.push_back(std::make_pair(0,-1));
    dirs.push_back(std::make_pair(0,1));

    do {
        // this code is used to make sure the numbers are odd
        int row = 1 + rand() % ((MAP_HEIGHT - 1) / 2) * 2;
        int col = 1 + rand() % ((MAP_WIDTH - 1) / 2) * 2;

        // Start tile.
        if (done == 0) {
            makeFloor(row, col);
        }

        if (impl_.map_[row][col]->terrain() == TERRAIN::FLOOR) {
            //Randomize Directions
            std::random_shuffle(dirs.begin(), dirs.end());

            bool blocked = true;

            do {
                if (rand() % 5 == 0) {
                    std::random_shuffle(dirs.begin(), dirs.end());
                }

                blocked = true;
                for (int i = 0; i < 4; i++) {
                    // Determine which direction the tile is
                    int r = row + dirs[i].first * 2;
                    int c = col + dirs[i].second * 2;
                    //Check to see if the tile can be used
                    if (r >= 1 && r < MAP_HEIGHT - 1 && c >= 1 && c < MAP_WIDTH - 1) {
                        if (impl_.map_[r][c]->terrain() != TERRAIN::FLOOR) {
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
    impl_.map_[row][col]->setTerrain(TERRAIN::FLOOR);
    impl_.map_[row][col]->setPassable(true);
    addItem(row, col);
}

void World::WorldImpl::addItem(int row, int col) {

    // Start space always empty
    if (row == 0 && col == startCol_) {
        return;
    // End space always dragon
    } else if (row == MAP_HEIGHT - 1 && col == endCol_) {
        Monster* dragon = new Monster("the", "dragon", ITEMTYPE::DRAGON, 1, 6, 6);
        items_[std::make_pair(row, col)] = ITEMPTR(dragon);
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
            items_[std::make_pair(row, col)] = ITEMPTR(monster);

        // item
        } else if (r < 90) {
            int r = rand() % 100;
            if (r < 40) {
                items_[std::make_pair(row, col)] = ITEMPTR(new Potion());
            } else if (r < 60) {
                items_[std::make_pair(row, col)] = ITEMPTR(new Key());
            } else if (r < 70) {
                items_[std::make_pair(row, col)] =
                    ITEMPTR(new Shield("a", "buckler", ITEMTYPE::SHIELD, 0, 1));
            } else if (r < 80) {
                items_[std::make_pair(row, col)] =
                    ITEMPTR(new Shield("a", "shield", ITEMTYPE::SHIELD, 0, 2));
            } else if (r < 90) {
                items_[std::make_pair(row, col)] =
                ITEMPTR(new Weapon("a", "sword", ITEMTYPE::WEAPON, 0, 1));
            } else {
                items_[std::make_pair(row, col)] =
                  ITEMPTR(new Weapon("a", "battleaxe", ITEMTYPE::WEAPON, 0, 2));
            }
            return;

        // trap
        } else {
            items_[std::make_pair(row, col)] = ITEMPTR(new Trap());
        }
    }
}

void World::WorldImpl::addDoors() {
    World world;

    for (int row = 1; row < MAP_HEIGHT - 1; row++) {
        for (int col = 1; col < MAP_WIDTH - 1; col++) {
            if (map_[row][col]->terrain() != TERRAIN::FLOOR) {
                continue;
            }
            int r = rand() % 100;
            if (r < 30) {
                // Check how many walls are adjacent to the door
                int adjacent = 0;
                if (map_[row - 1][col]->terrain() == TERRAIN::C_WALL) {
                    adjacent++;
                }
                if (map_[row + 1][col]->terrain() == TERRAIN::C_WALL) {
                    adjacent++;
                }
                if (map_[row][col - 1]->terrain() == TERRAIN::C_WALL) {
                    adjacent += 3;
                }
                if (map_[row][col + 1]->terrain() == TERRAIN::C_WALL) {
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
                items_[std::make_pair(row, col)] = ITEMPTR(door);
            }
        }
    }
}

void World::WorldImpl::addExits() {
    std::vector<int> freeCols;
    for (int i = 1; i < MAP_WIDTH - 1; i++) {
        if (impl_.map_[1][i]->terrain() == TERRAIN::FLOOR) {
            freeCols.push_back(i);
        }
    }
    startCol_ = freeCols[rand() % freeCols.size()];
    makeFloor(0, startCol_);

    playerRow_ = 0;
    playerCol_ = startCol_;

    freeCols.clear();
    for (int i = 1; i < MAP_WIDTH - 1; i++) {
        if (impl_.map_[MAP_HEIGHT - 2][i]->terrain() == TERRAIN::FLOOR) {
            freeCols.push_back(i);
        }
    }
    endCol_ = freeCols[rand() % freeCols.size()];
    makeFloor(MAP_HEIGHT - 1, endCol_);
}

void World::WorldImpl::addWalls() {
    //First pass puts a center wall adjacent to any floor or corridor.
    for (int row = 0; row < MAP_HEIGHT; row++) {
        for (int col = 0; col < MAP_WIDTH; col++) {

            auto & t = map_[row][col];

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

                    TERRAIN c = map_[x][y]->terrain();
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
    std::map<std::string, TERRAIN> walls = {
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

            auto & t = map_[row][col];

            if (t->terrain() != TERRAIN::C_WALL) {
                continue;
            }

            int count = 7;
            std::bitset<8> edgeset; // represent the edges as a binary number
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

                    if (map_[y][x]->isBlock()) {
                        edgeset.set(count);
                    }

                    auto item = items_.find(std::make_pair(y, x));
                    if (item != items_.end() &&
                    dynamic_cast<Door*>(item->second.get())) {
                        edgeset.set(count);
                    }

                    count--;
                }
            }

            for (auto& wall: walls) {
                std::bitset<8> mask(wall.first);
                if ((edgeset & mask) == mask) {
                    t->setTerrain(wall.second);
                }
            }
        }
    }
}
