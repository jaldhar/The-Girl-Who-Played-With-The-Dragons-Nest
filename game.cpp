#include <cstdlib>
#include <ctime>
#include <sstream>
#include <string>
using namespace std;

#include "armament.h"
#include "direction.h"
#include "door.h"
#include "game.h"
#include "item.h"
#include "key.h"
#include "monster.h"
#include "player.h"
#include "potion.h"
#include "trap.h"
#include "view.h"
#include "world.h"

struct Game::GameImpl {
string _name;
string _version;

bool canMove(int row, int col);
STATE fight();
STATE fightHere(int row, int col, Monster*& monster);
STATE batter();
STATE close();
STATE open();
STATE move();
STATE take();
STATE takeHere(int row, int col, Item*& item);
STATE directed(string command, function<STATE(GameImpl&)> func);

} Game::_impl;

static Player player;
static View view;
static World world;

int Game::run(const char *name, const char *version) {
    srand(time(NULL));

    _impl._name = name;
    _impl._version = version;

    STATE state = STATE::COMMAND;
    bool running = true;

    world.create();

    view.init(_impl._name);
    resize();

    Game::version();

    while (running) {
        switch(state) {
        case STATE::COMMAND:
            state = view.handleTopLevelInput(this);
            break;
        case STATE::FIGHTING:
            state = _impl.fight();
        case STATE::MOVING:
            state = _impl.move();
            break;
        case STATE::DEAD:
            state = dead();
            break;
        case STATE::QUIT:
            running = false;
            break;
        case STATE::ERROR:
        default:
            state = error();
            break;
        }
    }

    view.end();

    // Actually we won't ever get here because view.end() exit(3)s.
    return EXIT_SUCCESS;
}

STATE Game::badInput() {
    view.message("Huh?");
    return STATE::ERROR;
}

STATE Game::dead() {
    view.message("--press space to continue--");
    view.pause(this);
    return STATE::QUIT;
}

void Game::draw() {
    world.fov();
    view.draw(world, player);
}

STATE Game::error() {
    view.alert();

    return STATE::COMMAND;
}

STATE Game::fight() {
    player.setKeepFighting(false);
    return _impl.directed("fight", &GameImpl::fight);
}

STATE Game::fightToDeath() {
    player.setKeepFighting(true);
    return _impl.directed("fight to the death", &GameImpl::fight);
}

STATE Game::move_left() {
    player.setFacingY(0);
    player.setFacingX(-1);
    player.setPickup(true);
    player.setKeepMoving(false);
    return STATE::MOVING;
}

STATE Game::move_down() {
    player.setFacingY(1);
    player.setFacingX(0);
    player.setPickup(true);
    player.setKeepMoving(false);
    return STATE::MOVING;
}

STATE Game::move_up() {
    player.setFacingY(-1);
    player.setFacingX(0);
    player.setPickup(true);
    player.setKeepMoving(false);
    return STATE::MOVING;
}

STATE Game::move_right() {
    player.setFacingY(0);
    player.setFacingX(1);
    player.setPickup(true);
    player.setKeepMoving(false);
    return STATE::MOVING;
}

STATE Game::move_upleft() {
    player.setFacingY(-1);
    player.setFacingX(-1);
    player.setPickup(true);
    player.setKeepMoving(false);
    return STATE::MOVING;
}

STATE Game::move_upright() {
    player.setFacingY(-1);
    player.setFacingX(1);
    player.setPickup(true);
    player.setKeepMoving(false);
    return STATE::MOVING;
}

STATE Game::move_downleft() {
    player.setFacingY(1);
    player.setFacingX(-1);
    player.setPickup(true);
    player.setKeepMoving(false);
    return STATE::MOVING;
}

STATE Game::move_downright() {
    player.setFacingY(1);
    player.setFacingX(1);
    player.setPickup(true);
    player.setKeepMoving(false);
    return STATE::MOVING;
}

STATE Game::run_left() {
    player.setFacingY(0);
    player.setFacingX(-1);
    player.setPickup(true);
    player.setKeepMoving(true);
    return STATE::MOVING;
}

STATE Game::run_down() {
    player.setFacingY(1);
    player.setFacingX(0);
    player.setPickup(true);
    player.setKeepMoving(true);
    return STATE::MOVING;
}

STATE Game::run_up() {
    player.setFacingY(-1);
    player.setFacingX(0);
    player.setPickup(true);
    player.setKeepMoving(true);
    return STATE::MOVING;
}

STATE Game::run_right() {
    player.setFacingY(0);
    player.setFacingX(1);
    player.setPickup(true);
    player.setKeepMoving(true);
    return STATE::MOVING;
}

STATE Game::run_upleft() {
    player.setFacingY(-1);
    player.setFacingX(-1);
    player.setPickup(true);
    player.setKeepMoving(true);
    return STATE::MOVING;
}

STATE Game::run_upright() {
    player.setFacingY(-1);
    player.setFacingX(1);
    player.setPickup(true);
    player.setKeepMoving(true);
    return STATE::MOVING;
}

STATE Game::run_downleft() {
    player.setFacingY(1);
    player.setFacingX(-1);
    player.setPickup(true);
    player.setKeepMoving(true);
    return STATE::MOVING;
}

STATE Game::run_downright() {
    player.setFacingY(1);
    player.setFacingX(1);
    player.setPickup(true);
    player.setKeepMoving(true);
    return STATE::MOVING;
}

STATE Game::moveOver() {
    player.setPickup(false);
    return _impl.directed("move over", &GameImpl::move);
}

STATE Game::runOver() {
    player.setKeepMoving(true);
    player.setPickup(false);
    return _impl.directed("run over", &GameImpl::move);
}

STATE Game::batter() {
    return _impl.directed("batter down door", &GameImpl::batter);
}

STATE Game::open() {
    bool hasKey = false;
    player.foreach_carried([&](ITEMPTR& item) {
        Item* temp = item.get();
        if (dynamic_cast<Key*>(temp)) {
            hasKey = true;
        }
    });

    if (hasKey) {
        return _impl.directed("open door", &GameImpl::open);
    }
    view.message("You don't have the key.");
    return STATE::ERROR;
}

STATE Game::close() {
    return _impl.directed("close door", &GameImpl::close);
}

STATE Game::take() {
    return _impl.take();
}

STATE Game::drop() {
    if (world.itemAt(world.playerRow(), world.playerCol()) != nullptr) {
        view.message("You can't drop anything here.");
        return STATE::ERROR;
    }
    view.message("drop what?");
    int dropped = view.handleNumericalInput(this);
    if (dropped != 0) {
        Item* temp = player.drop(dropped);
        if (temp != nullptr) {
            world.insertItem(world.playerRow(), world.playerCol(), temp);
        }
    }
    return STATE::COMMAND;
}

STATE Game::wield() {
    view.message("wield what?");
    int dropped = view.handleNumericalInput(this);
    if (dropped > 2 && dropped < 7) {
        Item* temp = player.drop(dropped);
        if (temp != nullptr) {
            if (dynamic_cast<Armament*>(temp)) {
                if (player.wield(temp) == false) {
                    view.message("Your hands are full.");
                    player.carry(temp);
                }
            } else {
                view.message("You can't wield that.");
                player.carry(temp);
            }
        }
    }
    return STATE::COMMAND;

}

STATE Game::unwield() {
    view.message("unwield what?");
    int dropped = view.handleNumericalInput(this);
    if (dropped > 0 && dropped < 3) {
        Item* temp = player.drop(dropped);
        if (temp != nullptr) {
            if (player.carry(temp) == false) {
                view.message("You are carrying too much.");
                player.wield(temp);
            }
        }
    }
    return STATE::COMMAND;
}

STATE Game::quit() {
    view.message("Are you sure you want to quit? (y/n)");
    return view.handleBooleanInput(this) ? STATE::QUIT : STATE::COMMAND;
}

STATE Game::refresh() {
    view.refresh();

    return STATE::COMMAND;
}

STATE Game::resize() {
    view.resize(world);
    draw();
    return STATE::COMMAND;
}

STATE Game::shell() {
    view.shell();

    return STATE::COMMAND;
}

STATE Game::quaff() {
    bool quaffed = false;
    player.foreach_carried([&](ITEMPTR& item) {
        if (quaffed == false) {
            Item* temp = item.get();
            if (dynamic_cast<Potion*>(temp)) {
                player.setHealth(10 - player.health());
                delete item.release();
                quaffed = true;
            }
        }
    });

    if (quaffed) {
        return STATE::COMMAND;
    }

    view.message("You don't have any potions.");
    return STATE::ERROR;
}

STATE Game::version() {
    stringstream banner;

     banner <<  _impl._name << ' ' << _impl._version;
     view.message(banner.str());

     return STATE::COMMAND;
}

STATE Game::GameImpl::fight() {
    int row = world.playerRow() + player.facingY();
    int col = world.playerCol() + player.facingX();
    if (Monster* monster = dynamic_cast<Monster*>(world.itemAt(row, col))) {
        return fightHere(row, col, monster);
    }
    view.message("Nothing to fight here.");
    return STATE::ERROR;
}

STATE Game::GameImpl::fightHere(int row, int col, Monster*& monster) {
    stringstream output;

    int offenseBonus = 0, defenseBonus = 0;
    player.foreach_wielded([&](ITEMPTR& item) {
        Item* temp = item.get();
        if (temp != nullptr) {
            offenseBonus += dynamic_cast<Armament*>(temp)->offenseBonus();
            defenseBonus += dynamic_cast<Armament*>(temp)->defenseBonus();
        }
    });

    if (monster->attack() <= (player.defend() + defenseBonus)) {
        output << "The " << monster->name() << " misses you. ";
    } else {
        output << "The " << monster->name() << " hits you. ";
        player.setHealth(-1);
        if (monster->type() == ITEMTYPE::WIZARD) { // Teleport
            player.setKeepFighting(false);
            world.setPlayerRow(0);
            world.setPlayerCol(world.startCol());
        } else if (monster->type() == ITEMTYPE::DRAGON) {
            player.setHealth(-2);
        }
    }

    if ((player.attack() + offenseBonus) <= monster->defend()) {
        output << "You miss the " << monster->name() << ". ";
    } else {
        output << "You hit the " << monster->name() << ". ";
        monster->setHealth(-1);
    }

    STATE result;

    if (monster->health() < 1 ) {
        world.setPlayerRow(row);
        world.setPlayerCol(col);
        output << "You kill the "  << monster->name() << ". ";
        if (monster->type() == ITEMTYPE::DRAGON) {
            output << "You have won!";
            result = STATE::DEAD;
        } else {
            result = STATE::COMMAND;
        }
        world.removeItem(row, col, true);
        player.setKeepFighting(false);
    } else if (player.keepFighting()) {
        result =  STATE::FIGHTING;
    } else {
        player.setKeepFighting(false);
        result = STATE::COMMAND;
    }

    if ( player.health() < 1 ) {
        if (monster->type() == ITEMTYPE::TROLL) {
            output << "YHBT. YHL. HAND!";
        } else {
            output << "You are dead.";
        }
        player.setKeepFighting(false);
        result = STATE::DEAD;
    }

    view.message(output.str());

    return result;
}

STATE Game::GameImpl::batter() {
    int row = world.playerRow() + player.facingY();
    int col = world.playerCol() + player.facingX();

    if (dynamic_cast<Door*>(world.itemAt(row, col))) {
        view.message("You smash the door down.");
        world.removeItem(row, col, true);
        player.setHealth(-2);
        if (player.health() < 1) {
            view.message("You are dead.");
            return STATE::DEAD;
        }
        return STATE::COMMAND;
    }
    view.message("Nothing to batter down here.");
    return STATE::ERROR;

}

STATE Game::GameImpl::close() {
    int row = world.playerRow() + player.facingY();
    int col = world.playerCol() + player.facingX();

    if (Door* door = dynamic_cast<Door*>(world.itemAt(row, col))) {
        if (door->open() == false) {
            view.message("The door is already closed.");
            return STATE::ERROR;
        } else {
            door->setOpen(false);
        }
        return STATE::COMMAND;
    }
    view.message("Nothing to close here.");
    return STATE::ERROR;
}

STATE Game::GameImpl::open() {
    int row = world.playerRow() + player.facingY();
    int col = world.playerCol() + player.facingX();

    if (Door* door = dynamic_cast<Door*>(world.itemAt(row, col))) {
        if (door->open() == true) {
            view.message("The door is already open.");
            return STATE::ERROR;
        } else {
            door->setOpen(true);
        }
        return STATE::COMMAND;
    }
    view.message("Nothing to open here.");
    return STATE::ERROR;
}

bool Game::GameImpl::canMove(int row, int col) {
    if (row < 0
        || row >= world.height()
        || col < 0
        || col >= world.width()) {
        return false;
    }

    Tile* t = world.tileAt( row, col );
    if(t->passable() == false) {
        return false;
    }

    return true;
}

STATE Game::GameImpl::move() {
    int row = world.playerRow() + player.facingY();
    int col = world.playerCol() + player.facingX();
    if(canMove(row, col) == false) {
        if (player.keepMoving()){
            player.setKeepMoving(false);
            return STATE::COMMAND;
        } else {
            view.message("You can't go there!");
            return STATE::ERROR;
        }
    }

    if (Item *item = world.itemAt(row, col)) {

        if (Door* d = dynamic_cast<Door*>(item)) {
            if (d->open() == false) {
                view.message("The door is shut.");
                return STATE::ERROR;
            }

        } else if (Trap* trap = dynamic_cast<Trap*>(item)) {
            if (player.pickup()) {
                view.message("You have stepped in a trap.");
                player.setHealth(-2);
                if (player.health() < 1) {
                    view.message("You are dead.");
                    return STATE::DEAD;
                }
                trap->setSprung(true);
            }

        } else if (Monster* monster = dynamic_cast<Monster*>(item)) {
                 return fightHere(row, col, monster);

        } else {
            if (player.pickup()) {
                return takeHere(row, col, item);
            }
        }
    }
    world.setPlayerRow(row);
    world.setPlayerCol(col);

    return player.keepMoving() ? STATE::MOVING : STATE::COMMAND;
}

STATE Game::GameImpl::take() {
    int row = world.playerRow();
    int col = world.playerCol();

    Item* item = world.itemAt(row, col);

    if (item != nullptr) {
        return takeHere(row, col, item);
    }

    view.message("Nothing to take here.");
    return STATE::ERROR;
}

STATE Game::GameImpl::takeHere(int row, int col, Item*& item) {
    if (dynamic_cast<Monster*>(item) || dynamic_cast<Door*>(item) || dynamic_cast<Trap*>(item)) {
        view.message("You can't take that!");
        return STATE::ERROR;
    } else {
        if (player.carry(item) == false) {
            view.message("You are carrying too much.");
            return STATE::ERROR;
        }
    }

    world.removeItem(row, col);
    world.setPlayerRow(row);
    world.setPlayerCol(col);
    return STATE::COMMAND;
}

STATE Game::GameImpl::directed(string command, function<STATE(GameImpl&)> func) {

    stringstream prompt;
    prompt << command << " in which direction?";
    view.message(prompt.str());
    Game game;

    switch(view.handleDirectionInput(&game)) {
        case DIRECTION::NORTH:
            player.setFacingY(-1);
            player.setFacingX(0);
            return func(_impl);
            break;
        case DIRECTION::EAST:
            player.setFacingY(0);
            player.setFacingX(1);
            return func(_impl);
            break;
        case DIRECTION::WEST:
            player.setFacingY(0);
            player.setFacingX(-1);
            return func(_impl);
            break;
        case DIRECTION::SOUTH:
            player.setFacingY(1);
            player.setFacingX(0);
            return func(_impl);
            break;
        case DIRECTION::NORTHWEST:
            player.setFacingY(-1);
            player.setFacingX(-1);
            return func(_impl);
            break;
        case DIRECTION::NORTHEAST:
            player.setFacingY(-1);
            player.setFacingX(1);
            return func(_impl);
            break;
        case DIRECTION::SOUTHWEST:
            player.setFacingY(1);
            player.setFacingX(-1);
            return func(_impl);
            break;
        case DIRECTION::SOUTHEAST:
            player.setFacingY(1);
            player.setFacingX(1);
            return func(_impl);
            break;
        case DIRECTION::CANCELLED:
            view.message("");
            return STATE::COMMAND;
            break;
        case DIRECTION::NO_DIRECTION:
        default:
            view.message("Huh?");
            return STATE::ERROR;
            break;
    }
    return STATE::COMMAND;
}
