#include <clocale>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <ctime>
#include <deque>
#include <functional>
#include <map>
using namespace std;

#include <curses.h>
// These ncurses macros name clash with c++ symbols on old versions of ncurses
#if NCURSES_MAJOR_VERSION < 5 || (NCURSES_MAJOR_VERSION == 5 && NCURSES_MINOR_VERSION < 9)
#undef box
#undef clear
#undef erase
#undef move
#undef refresh
#endif

#include "door.h"
#include "item.h"
#include "terrain.h"
#include "trap.h"
#include "view.h"

typedef map<int, function<STATE(Game*)>> COMMANDMAP;
typedef map<int, DIRECTION> DIRECTIONMAP;
typedef map<ITEMTYPE, chtype> ITEMMAP;
typedef map<TERRAIN, chtype> TILEMAP;

static const int TILEHEIGHT = 1;
static const int TILEWIDTH  = 1;
static const int BEATS_PER_SECOND = 50;
static const size_t MESSAGEWINHEIGHT = 15;

struct View::ViewImpl {
    ViewImpl()=default;
    ~ViewImpl()=default;

    void    drawActors(World& world, int top, int left);
    void    drawInventory(Player& player);
    void    drawItems(World& world, int top, int left, int height, int width);
    void    drawMessage();
    void    drawTitle();
    void    drawViewport(World& world);
    bool    oneBeatPassed();
    void    setTitleWin(WINDOW*& win);

    static int  createTitleWin(WINDOW*, int);
    static void end_sig(int);

    WINDOW*      _inventory;
    WINDOW*      _message;
    WINDOW*      _title;
    WINDOW*      _viewport;
    COMMANDMAP   _commandkeys;
    DIRECTIONMAP _directionkeys;
    ITEMMAP      _itemmap;
    TILEMAP      _tilemap;
    int          _lines;
    int          _cols;
    clock_t      _lastTick;
    string       _titleText;
    deque<string> _messages;
} View::_impl;

void View::alert() {
    beep();
}

STATE View::draw(World &world, Player &player) {
    curs_set(0);
    wclear(stdscr);

    _impl.drawTitle();

    mvhline(0, 4, ACS_CKBOARD, _impl._cols - 4 - 4);

    mvvline(0, 4, ACS_CKBOARD, 23);
    _impl.drawViewport(world);

    mvvline(0, 20, ACS_CKBOARD, 17);
    _impl.drawMessage();

    mvvline(0, _impl._cols - 4 - 1, ACS_CKBOARD, 23);

    mvhline(16, 4, ACS_CKBOARD, _impl._cols - 4 - 4);
    _impl.drawInventory(player);

    mvhline(22, 4, ACS_CKBOARD, _impl._cols - 4 - 4);


    doupdate();
    return STATE::COMMAND;
}

void View::end() {
    curs_set(1);
    endwin();
    clear();
    exit(EXIT_SUCCESS);
}

STATE View::handleTopLevelInput(Game *game) {
    int c;
    while (!_impl.oneBeatPassed()) {
        if ((c = getch()) != ERR) {
            auto it = _impl._commandkeys.find(c);
            if (it != _impl._commandkeys.end()) {
                return (it->second)(game);
            }

            return game->badInput();
        }

    }
    return STATE::DRAW;
}

DIRECTION View::handleDirectionInput() {
    int c;

    while (true) {
        if ((c = getch()) != ERR) {
            auto it = _impl._directionkeys.find(c);
            if (it != _impl._directionkeys.end()) {
                return it->second;
            }
            return DIRECTION::NO_DIRECTION;
        }
    }
}

int View::handleNumericalInput() {
    int c;

    while (true) {
        if ((c = getch()) != ERR) {
            c -= '0';
            if (c > 0 || c < 7) {
                return c;
            }
            return 0;
        }
    }
}

void View::init(string titleText) {
    setlocale(LC_ALL, "POSIX");

    struct sigaction act;
    act.sa_handler = View::ViewImpl::end_sig;
    sigemptyset (&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGSEGV, &act, NULL);

    _impl._inventory = NULL;
    _impl._message = NULL;
    _impl._title = NULL;
    _impl._viewport = NULL;
    _impl._lines = 0;
    _impl._cols = 0;

    _impl._commandkeys[0x12] /* CTRL-R */ = &Game::refresh;
    _impl._commandkeys[KEY_RESIZE]        = &Game::resize;
    _impl._commandkeys['h']               = &Game::move_left;
    _impl._commandkeys[KEY_LEFT]          = &Game::move_left;
    _impl._commandkeys['j']               = &Game::move_down;
    _impl._commandkeys[KEY_DOWN]          = &Game::move_down;
    _impl._commandkeys['k']               = &Game::move_up;
    _impl._commandkeys[KEY_UP]            = &Game::move_up;
    _impl._commandkeys['l']               = &Game::move_right;
    _impl._commandkeys[KEY_RIGHT]         = &Game::move_right;
    _impl._commandkeys['y']               = &Game::move_upleft;
    _impl._commandkeys[KEY_HOME]          = &Game::move_upleft;
    _impl._commandkeys['u']               = &Game::move_upright;
    _impl._commandkeys[KEY_PPAGE]         = &Game::move_upright;
    _impl._commandkeys['b']               = &Game::move_downleft;
    _impl._commandkeys[KEY_END]           = &Game::move_downleft;
    _impl._commandkeys['n']               = &Game::move_downright;
    _impl._commandkeys[KEY_NPAGE]         = &Game::move_downright;
    _impl._commandkeys['H']               = &Game::run_left;
    _impl._commandkeys['J']               = &Game::run_down;
    _impl._commandkeys['K']               = &Game::run_up;
    _impl._commandkeys['L']               = &Game::run_right;
    _impl._commandkeys['Y']               = &Game::run_upleft;
    _impl._commandkeys['U']               = &Game::run_upright;
    _impl._commandkeys['B']               = &Game::run_downleft;
    _impl._commandkeys['N']               = &Game::run_downright;
    _impl._commandkeys['m']               = &Game::moveOver;
    _impl._commandkeys['M']               = &Game::runOver;
    _impl._commandkeys['c']               = &Game::close;
    _impl._commandkeys['d']               = &Game::drop;
    _impl._commandkeys['f']               = &Game::fight;
    _impl._commandkeys['F']               = &Game::fightToDeath;
    _impl._commandkeys['o']               = &Game::open;
    _impl._commandkeys['O']               = &Game::batter;
    _impl._commandkeys['q']               = &Game::quaff;
    _impl._commandkeys['Q']               = &Game::quit;
    _impl._commandkeys['u']               = &Game::unwield;
    _impl._commandkeys['v']               = &Game::version;
    _impl._commandkeys['w']               = &Game::wield;
    _impl._commandkeys[',']               = &Game::take;
    _impl._commandkeys['!']               = &Game::shell;

    _impl._directionkeys['h']             = DIRECTION::WEST;
    _impl._directionkeys[KEY_LEFT]        = DIRECTION::WEST;
    _impl._directionkeys['j']             = DIRECTION::SOUTH;
    _impl._directionkeys[KEY_DOWN]        = DIRECTION::SOUTH;
    _impl._directionkeys['k']             = DIRECTION::NORTH;
    _impl._directionkeys[KEY_UP]          = DIRECTION::NORTH;
    _impl._directionkeys['l']             = DIRECTION::EAST;
    _impl._directionkeys[KEY_RIGHT]       = DIRECTION::EAST;
    _impl._directionkeys['y']             = DIRECTION::NORTHWEST;
    _impl._directionkeys[KEY_HOME]        = DIRECTION::NORTHWEST;
    _impl._directionkeys['u']             = DIRECTION::NORTHEAST;
    _impl._directionkeys[KEY_PPAGE]       = DIRECTION::NORTHEAST;
    _impl._directionkeys['b']             = DIRECTION::SOUTHWEST;
    _impl._directionkeys[KEY_END]         = DIRECTION::SOUTHWEST;
    _impl._directionkeys['n']             = DIRECTION::SOUTHEAST;
    _impl._directionkeys[KEY_NPAGE]       = DIRECTION::SOUTHEAST;
    _impl._directionkeys[0x1b]/* ESCAPE */= DIRECTION::CANCELLED;

    ripoffline(1, View::ViewImpl::createTitleWin);
    initscr();
    cbreak();
    noecho();
    nonl();
    keypad(stdscr, TRUE);
    intrflush(stdscr, FALSE);
    nodelay(stdscr, TRUE);

    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_BLACK, COLOR_WHITE);     // message/status window
        init_pair(2, COLOR_WHITE, COLOR_BLACK);     // walls
        init_pair(3, COLOR_BLACK,  COLOR_CYAN);     // title window
        init_pair(4, COLOR_BLACK,  COLOR_BLACK);    // background
        init_pair(5, COLOR_RED,  COLOR_BLACK);      // player and items
        init_pair(6, COLOR_MAGENTA,   COLOR_BLACK); // monsters
        init_pair(7, COLOR_YELLOW,   COLOR_BLACK);  // doors
    }

    _impl._tilemap[TERRAIN::EMPTY] = ' ';    //use of ACS_* requires this goes
    _impl._tilemap[TERRAIN::CORRIDOR] = '.'; // after call to initscr().
    _impl._tilemap[TERRAIN::FLOOR]    = '.';
    _impl._tilemap[TERRAIN::C_WALL]   = '+';
    _impl._tilemap[TERRAIN::H_WALL]   = ACS_HLINE;
    _impl._tilemap[TERRAIN::V_WALL]   = ACS_VLINE;
    _impl._tilemap[TERRAIN::UL_WALL]  = ACS_ULCORNER;
    _impl._tilemap[TERRAIN::UR_WALL]  = ACS_URCORNER;
    _impl._tilemap[TERRAIN::LR_WALL]  = ACS_LRCORNER;
    _impl._tilemap[TERRAIN::LL_WALL]  = ACS_LLCORNER;
    _impl._tilemap[TERRAIN::TT_WALL]  = ACS_TTEE;
    _impl._tilemap[TERRAIN::RT_WALL]  = ACS_RTEE;
    _impl._tilemap[TERRAIN::BT_WALL]  = ACS_BTEE;
    _impl._tilemap[TERRAIN::LT_WALL]  = ACS_LTEE;
    _impl._tilemap[TERRAIN::PLAYER]   = '@';
    _impl._tilemap[TERRAIN::H_DOOR_OPEN]   = '/';
    _impl._tilemap[TERRAIN::H_DOOR_CLOSED] = ACS_HLINE;
    _impl._tilemap[TERRAIN::V_DOOR_OPEN]   = '/';
    _impl._tilemap[TERRAIN::V_DOOR_CLOSED] = ACS_VLINE;
    _impl._tilemap[TERRAIN::TRAP]          = '^';

    _impl._itemmap[ITEMTYPE::NOTHING]     = ' ';
    _impl._itemmap[ITEMTYPE::BAT]         = 'B';
    _impl._itemmap[ITEMTYPE::CUBE]        = 'C';
    _impl._itemmap[ITEMTYPE::DRAGON]      = 'D';
    _impl._itemmap[ITEMTYPE::FLOATINGEYE] = 'F';
    _impl._itemmap[ITEMTYPE::HOBGOBLIN]   = 'H';
    _impl._itemmap[ITEMTYPE::KOBOLD]      = 'K';
    _impl._itemmap[ITEMTYPE::LIZARDMAN]   = 'L';
    _impl._itemmap[ITEMTYPE::MINOTAUR]    = 'M';
    _impl._itemmap[ITEMTYPE::NAGA]        = 'N';
    _impl._itemmap[ITEMTYPE::ORC]         = 'O';
    _impl._itemmap[ITEMTYPE::RAT]         = 'R';
    _impl._itemmap[ITEMTYPE::SPIDER]      = 'S';
    _impl._itemmap[ITEMTYPE::TROLL]       = 'T';
    _impl._itemmap[ITEMTYPE::WIZARD]      = 'W';
    _impl._itemmap[ITEMTYPE::ZOMBIE]      = 'Z';
    _impl._itemmap[ITEMTYPE::SHIELD]      = ']';
    _impl._itemmap[ITEMTYPE::WEAPON]      = ')';
    _impl._itemmap[ITEMTYPE::POTION]      = '!';
    _impl._itemmap[ITEMTYPE::KEY]         = 'k';
    _impl._titleText = titleText;

    _impl._lastTick = clock();
}

void View::message(const char *msg) {
    if (_impl._messages.size() == MESSAGEWINHEIGHT) {
        _impl._messages.pop_front();
    }
    _impl._messages.push_back(msg);
}

void View::pause() {
    int c;
    do {
        c = getch();
    } while (c != ' ');
}

void View::refresh() {
    redrawwin(_impl._title);
    redrawwin(_impl._viewport);
    redrawwin(_impl._message);
    redrawwin(_impl._inventory);
    doupdate();
}

void View::resize(World& world) {
    getmaxyx(stdscr, _impl._lines, _impl._cols);

    wbkgd(stdscr, ' ');

    if (_impl._viewport != NULL) {
        delwin(_impl._viewport);
        _impl._viewport = NULL;
    }
    _impl._viewport = subwin(stdscr, world.height(), world.width(), 1, 5);
    wbkgd(_impl._viewport, ' ' | COLOR_PAIR(4));

    if (_impl._message != NULL) {
        delwin(_impl._message);
        _impl._message = NULL;
    }
    _impl._message = subwin(stdscr, world.height(), _impl._cols - 4 - 4 - 2 - world.width(), 1, 21);
    scrollok(_impl._message, TRUE);
    wbkgd(_impl._message, ' ' | COLOR_PAIR(1));

    if (_impl._inventory != NULL) {
        delwin(_impl._inventory);
        _impl._inventory = NULL;
    }
    _impl._inventory = subwin(stdscr, 5, _impl._cols - 4 - 4 - 2, 17, 5);
    wbkgd(_impl._inventory, ' ' | COLOR_PAIR(1));

    wresize(_impl._title, 1, _impl._cols);
    wbkgd(_impl._title, ' ' | COLOR_PAIR(3));
}

void View::shell() {
    def_prog_mode();
    endwin();
    fprintf(stderr, "Type 'exit' to return.\n");
    int returncode = system("/bin/sh");
    returncode += 0; // stops g++ warning for set but unused variable.
    reset_prog_mode();
}

// Private methods

int View::ViewImpl::createTitleWin(WINDOW* win, int /* ncols */) {
    _impl.setTitleWin(win);

    return 0;
}

void View::ViewImpl::drawActors(World& world, int top, int left) {
    mvwaddch(_viewport, world.playerRow() - top, world.playerCol() - left,
        _tilemap[TERRAIN::PLAYER] | COLOR_PAIR(5) | A_BOLD);
}

void View::ViewImpl::drawInventory(Player& player) {
    wclear(_inventory);
    mvwaddstr(_inventory, 0, 5, "wielding");
    mvwaddstr(_inventory, 0, 30, "carrying");
    mvwprintw(_inventory, 0, 55, "stamina: %02d", player.health());
    int row = 1;
    int key = 1;
    player.foreach_wielded([&](ITEMPTR& item) {
        Item* temp = item.get();
        string name;
        if (temp == nullptr) {
            name = "nothing";
        } else {
            name = temp->article() + " " + temp->name();
        }
        mvwprintw(_inventory, row++, 5, "%d %s", key++, name.c_str());
    });
    row = 1;
    player.foreach_carried([&](ITEMPTR& item) {
        Item* temp = item.get();
        string article, name;
        if (temp == nullptr) {
            name = "nothing";
        } else {
            name = temp->article() + " " + temp->name();
        }
        mvwprintw(_inventory, row++, 30, "%d %s", key++, name.c_str());
    });
}

void View::ViewImpl::drawItems(World& world, int top, int left, int height, int width) {
    world.foreach_item(top, left, height, width, [=](int row, int col, ITEMPTR& item) {
        chtype t;

        switch(item->type()) {
            case ITEMTYPE::DOOR: {
                    auto d = dynamic_cast<Door*>(item.get());
                    if (d->open()) {
                        t = (d->horizontal()) ? _tilemap[TERRAIN::H_DOOR_OPEN] | COLOR_PAIR(7)
                            : _tilemap[TERRAIN::V_DOOR_OPEN] | COLOR_PAIR(7);
                    } else {
                        t = (d->horizontal()) ? _tilemap[TERRAIN::H_DOOR_CLOSED] | COLOR_PAIR(7)
                            : _tilemap[TERRAIN::V_DOOR_CLOSED] | COLOR_PAIR(7);
                    }
                }
                break;
            case ITEMTYPE::TRAP: {
                    auto trap = dynamic_cast<Trap*>(item.get());
                    if (trap->sprung()) {
                        t = _tilemap[TERRAIN::TRAP]  | COLOR_PAIR(7);
                    } else {
                        t = _tilemap[TERRAIN::FLOOR] | COLOR_PAIR(2);
                    }

                }
                break;
            default: {
                    t = _itemmap[item->type()] | COLOR_PAIR(7);
                }
                break;
        }
        mvwaddch(_viewport, row - top, col - left, t);
    });
}

void View::ViewImpl::drawMessage() {
    wclear(_message);
    for(auto & msg : _messages) {
        wprintw(_message, "%s\n", msg.c_str());
    }
    wnoutrefresh(_message);
}

void View::ViewImpl::drawTitle() {
    wclear(_title);
    const int len = _titleText.length();
    mvwaddstr(_title, 0, (_impl._cols - len)/2, _titleText.c_str());
    wnoutrefresh(_title);
}

void View::ViewImpl::drawViewport(World &world) {
    int screenHeight, screenWidth;
    getmaxyx(_viewport, screenHeight, screenWidth);

    wbkgd(_viewport, _tilemap[TERRAIN::EMPTY]);

    int playerCol = world.playerCol();
    int playerRow = world.playerRow();
    int worldHeight = world.height();
    int worldWidth  = world.width();

    int top = playerRow - (screenHeight) / 2;
    int left = playerCol - (screenWidth) / 2;

    for (int row = 0; row < screenHeight; row += TILEHEIGHT) {
        int mapRow = row + top;

        if (mapRow < 0 || mapRow >= worldHeight) {
            continue;
        }

        for (int  col = 0; col < screenWidth; col += TILEWIDTH) {
            int mapCol = col + left;

            if (mapCol < 0 || mapCol >= worldWidth) {
                continue;
            }

            chtype display;
            Tile* t = world.tileAt(mapRow, mapCol);

            if (t->visible()) {
                display = _tilemap[t->terrain()];

                if (t->isBlock()) {
                    display |= COLOR_PAIR(2);
                }

            } else {
                continue;
            }

            mvwaddch(_viewport, row, col, display);
        }
    }

    drawItems(world, top, left, screenHeight, screenWidth);

    drawActors(world, top, left);

    wnoutrefresh(_viewport);
}

void View::ViewImpl::end_sig(int /* sig */) {
    View view;
    view.end();
}

bool View::ViewImpl::oneBeatPassed() {
    clock_t tick = clock();

    if (tick - _lastTick > CLOCKS_PER_SEC / BEATS_PER_SECOND) {
        _lastTick = tick;
        return true;
    }
    return false;
}

void View::ViewImpl::setTitleWin(WINDOW*& win) {

    _title = win;
}
