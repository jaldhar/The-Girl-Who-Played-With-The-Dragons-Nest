#ifndef GAME_H
#define GAME_H

#include "state.h"

class Game {
public:
    Game()=default;
    ~Game()=default;
    int run(const char *name, const char *version);
    STATE badInput();
    STATE dead();
    void  draw();
    STATE error();
    STATE fight();
    STATE fightToDeath();
    STATE move_left();
    STATE move_down();
    STATE move_up();
    STATE move_right();
    STATE move_upleft();
    STATE move_upright();
    STATE move_downleft();
    STATE move_downright();
    STATE run_left();
    STATE run_down();
    STATE run_up();
    STATE run_right();
    STATE run_upleft();
    STATE run_upright();
    STATE run_downleft();
    STATE run_downright();
    STATE moveOver();
    STATE runOver();
    STATE batter();
    STATE open();
    STATE close();
    STATE take();
    STATE drop();
    STATE wield();
    STATE unwield();
    STATE quaff();
    STATE quit();
    STATE refresh();
    STATE resize();
    STATE shell();
    STATE version();
private:
    struct GameImpl;
    static GameImpl _impl;
};

#endif // GAME_H
