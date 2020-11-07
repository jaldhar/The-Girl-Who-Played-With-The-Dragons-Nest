#include <algorithm>
#include <clocale>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <ctime>
#include <deque>
#include <functional>
#include <map>
#include <sstream>

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
#include "monster.h"
#include "terrain.h"
#include "trap.h"
#include "view.h"

struct WindowDeleter {
    void operator()(WINDOW* window) {
        delwin(window);
    }
};

using WindowPtr = std::unique_ptr<WINDOW, WindowDeleter>;
using CommandMap = std::map<int, std::function<STATE(Game*)>>;
using DirectionMap = std::map<int, DIRECTION>;
using ItemMap = std::map<ITEMTYPE, chtype>;
using TileMap = std::map<TERRAIN, chtype>;

constexpr int TILEHEIGHT = 1;
constexpr int TILEWIDTH  = 1;
constexpr int BEATS_PER_SECOND = 50;
constexpr std::size_t MESSAGEWINHEIGHT = 15;

struct View::ViewImpl {
    ViewImpl();

    ~ViewImpl()=default;

    void    drawActors(WINDOW* viewport, World& world, int top, int left);
    void    drawInventory(Player& player);
    void    drawItems(WINDOW* viewport, World& world, int top, int left, int height, int width);
    void    drawMessage();
    void    drawTitle();
    void    drawViewport(World& world);
    bool    oneBeatPassed();
    void    setTitleWin(WINDOW*& win);

    static int  createTitleWin(WINDOW*, int);
    static void end_sig(int);

    WindowPtr               inventory_;
    WindowPtr               message_;
    WindowPtr               title_;
    WindowPtr               viewport_;
    CommandMap              commandkeys_;
    DirectionMap            directionkeys_;
    ItemMap                 itemmap_;
    TileMap                 tilemap_;
    int                     lines_;
    int                     cols_;
    std::size_t             messageWinWidth_;
    std::clock_t            lastTick_;
    std::string             titleText_;
    std::deque<std::string> messages_;
} View::impl_;

void View::alert() {
    beep();
}

STATE View::draw(World &world, Player &player) {
    curs_set(0);
    werase(stdscr);

    impl_.drawTitle();

    mvhline(0, 4, ACS_CKBOARD, impl_.cols_ - 4 - 4);

    mvvline(0, 4, ACS_CKBOARD, 23);
    impl_.drawViewport(world);

    mvvline(0, 20, ACS_CKBOARD, 17);
    impl_.drawMessage();

    mvvline(0, impl_.cols_ - 4 - 1, ACS_CKBOARD, 23);

    mvhline(16, 4, ACS_CKBOARD, impl_.cols_ - 4 - 4);
    impl_.drawInventory(player);

    mvhline(22, 4, ACS_CKBOARD, impl_.cols_ - 4 - 4);


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

    while (true) {
        if ((c = getch()) != ERR) {
            auto it = impl_.commandkeys_.find(c);
            if (it != impl_.commandkeys_.end()) {
                return (it->second)(game);
            }

            return game->badInput();
        }
        if (impl_.oneBeatPassed()) {
            game->draw();
        }

    }
}

DIRECTION View::handleDirectionInput(Game* game) {
    int c;

    while (true) {
        if ((c = getch()) != ERR) {
            auto it = impl_.directionkeys_.find(c);
            if (it != impl_.directionkeys_.end()) {
                return it->second;
            }
            return DIRECTION::NO_DIRECTION;
        }
        if (impl_.oneBeatPassed()) {
            game->draw();
        }
    }
}

int View::handleNumericalInput(Game* game) {
    int c;

    while (true) {
        if ((c = getch()) != ERR) {
            c -= '0';
            if (c > 0 || c <= 9) {
                return c;
            }
            return 0;
        }
        if (impl_.oneBeatPassed()) {
            game->draw();
        }
    }
}

bool View::handleBooleanInput(Game* game) {
    int c;

    while (true) {
        if ((c = getch()) != ERR) {
            if (toupper(c) == 'Y') {
                return true;
            }
            return false;
        }
        if (impl_.oneBeatPassed()) {
            game->draw();
        }
    }
}

void View::init(std::string titleText) {
    std::setlocale(LC_ALL, "POSIX");

    struct sigaction act;
    act.sa_handler = View::ViewImpl::end_sig;
    sigemptyset (&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGSEGV, &act, NULL);

    impl_.titleText_ = titleText;

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
        init_pair(7, COLOR_GREEN,   COLOR_BLACK);  // doors
    }

    impl_.tilemap_[TERRAIN::EMPTY] = ' ';    //use of ACS_* requires this goes
    impl_.tilemap_[TERRAIN::CORRIDOR] = '.'; // after call to initscr().
    impl_.tilemap_[TERRAIN::FLOOR]    = '.';
    impl_.tilemap_[TERRAIN::C_WALL]   = '+';
    impl_.tilemap_[TERRAIN::H_WALL]   = ACS_HLINE;
    impl_.tilemap_[TERRAIN::V_WALL]   = ACS_VLINE;
    impl_.tilemap_[TERRAIN::UL_WALL]  = ACS_ULCORNER;
    impl_.tilemap_[TERRAIN::UR_WALL]  = ACS_URCORNER;
    impl_.tilemap_[TERRAIN::LR_WALL]  = ACS_LRCORNER;
    impl_.tilemap_[TERRAIN::LL_WALL]  = ACS_LLCORNER;
    impl_.tilemap_[TERRAIN::TT_WALL]  = ACS_TTEE;
    impl_.tilemap_[TERRAIN::RT_WALL]  = ACS_RTEE;
    impl_.tilemap_[TERRAIN::BT_WALL]  = ACS_BTEE;
    impl_.tilemap_[TERRAIN::LT_WALL]  = ACS_LTEE;
    impl_.tilemap_[TERRAIN::PLAYER]   = '@';
    impl_.tilemap_[TERRAIN::H_DOOR_OPEN]   = '/';
    impl_.tilemap_[TERRAIN::H_DOOR_CLOSED] = ACS_HLINE;
    impl_.tilemap_[TERRAIN::V_DOOR_OPEN]   = '/';
    impl_.tilemap_[TERRAIN::V_DOOR_CLOSED] = ACS_VLINE;
    impl_.tilemap_[TERRAIN::TRAP]          = '^';
}

void View::message(std::string msg) {
    std::istringstream words(msg);
    std::ostringstream wrapped;
    std::string word;

    if (words >> word) {
        wrapped << word;
        size_t space_left = impl_.messageWinWidth_ - word.length();
        while (words >> word) {
            if (space_left < word.length() + 1) {
                wrapped << '\n' << word;
                space_left = impl_.messageWinWidth_ - word.length();
            } else {
                wrapped << ' ' << word;
                space_left -= word.length() + 1;
            }
        }
    }
    std::string temp;
    std::istringstream lines(wrapped.str());
    while (std::getline(lines, temp)) {
        impl_.messages_.push_back(temp);
    }
    while (impl_.messages_.size() >  MESSAGEWINHEIGHT) {
        impl_.messages_.pop_front();
    }
}

void View::pause(Game* game) {
    int c;

    while (true) {
        if ((c = getch()) == ' ') {
            return;
        }
        if (impl_.oneBeatPassed()) {
            game->draw();
        }
    }
}

void View::refresh() {
    redrawwin(impl_.title_.get());
    redrawwin(impl_.viewport_.get());
    redrawwin(impl_.message_.get());
    redrawwin(impl_.inventory_.get());
    doupdate();
}

void View::resize(World& world) {
    getmaxyx(stdscr, impl_.lines_, impl_.cols_);

    wbkgd(stdscr, ' ');

    impl_.viewport_.reset(subwin(stdscr, world.height(), world.width(), 1, 5));
    auto viewport = impl_.viewport_.get();
    wbkgd(viewport, ' ' | COLOR_PAIR(4));


    // COLS - left margin - right margin - sub window borders - world width
    impl_.messageWinWidth_ = impl_.cols_ - 4 - 4 - 3  - world.width();
    impl_.message_.reset(subwin(stdscr, world.height(), impl_.messageWinWidth_,
        1, 21));
    auto message = impl_.message_.get();
    wbkgd(message, ' ' | COLOR_PAIR(1));
    scrollok(message, TRUE);
    idlok(message, TRUE);

    impl_.inventory_.reset(subwin(stdscr, 5, impl_.cols_ - 4 - 4 - 2, 17, 5));
    auto inventory = impl_.inventory_.get();
    wbkgd(inventory, ' ' | COLOR_PAIR(1));

    auto title = impl_.title_.get();
    wresize(title, 1, impl_.cols_);
    wbkgd(title, ' ' | COLOR_PAIR(3));
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

View::ViewImpl::ViewImpl() :
inventory_{nullptr}, message_{nullptr}, title_{nullptr}, viewport_{nullptr},
commandkeys_{
    { 0x12, /* CTRL-R */    &Game::refresh },
    { KEY_RESIZE,           &Game::resize },
    { 'h',                  &Game::move_left },
    { KEY_LEFT,             &Game::move_left },
    { 'j',                  &Game::move_down },
    { KEY_DOWN,             &Game::move_down },
    { 'k',                  &Game::move_up },
    { KEY_UP,               &Game::move_up },
    { 'l',                  &Game::move_right },
    { KEY_RIGHT,            &Game::move_right },
    { 'y',                  &Game::move_upleft },
    { KEY_HOME,             &Game::move_upleft },
    { 'u',                  &Game::move_upright },
    { KEY_PPAGE,            &Game::move_upright },
    { 'b',                  &Game::move_downleft },
    { KEY_END,              &Game::move_downleft },
    { 'n',                  &Game::move_downright },
    { KEY_NPAGE,            &Game::move_downright },
    { 'H',                  &Game::run_left },
    { 'J',                  &Game::run_down },
    { 'K',                  &Game::run_up },
    { 'L',                  &Game::run_right },
    { 'Y',                  &Game::run_upleft },
    { 'U',                  &Game::run_upright },
    { 'B',                  &Game::run_downleft },
    { 'N',                  &Game::run_downright },
    { 'm',                  &Game::moveOver },
    { 'M',                  &Game::runOver },
    { 'c',                  &Game::close },
    { 'd',                  &Game::drop },
    { 'f',                  &Game::fight },
    { 'F',                  &Game::fightToDeath },
    { 'o',                  &Game::open },
    { 'O',                  &Game::batter },
    { 'q',                  &Game::quaff },
    { 'Q',                  &Game::quit },
    { 'U',                  &Game::unwield },
    { 'v',                  &Game::version },
    { 'w',                  &Game::wield },
    { ',',                  &Game::take },
    { '!',                  &Game::shell },
},
directionkeys_{
    { 'h',              DIRECTION::WEST },
    { KEY_LEFT,         DIRECTION::WEST },
    { 'j',              DIRECTION::SOUTH },
    { KEY_DOWN,         DIRECTION::SOUTH },
    { 'k',              DIRECTION::NORTH },
    { KEY_UP,           DIRECTION::NORTH },
    { 'l',              DIRECTION::EAST },
    { KEY_RIGHT,        DIRECTION::EAST },
    { 'y',              DIRECTION::NORTHWEST },
    { KEY_HOME,         DIRECTION::NORTHWEST },
    { 'u',              DIRECTION::NORTHEAST },
    { KEY_PPAGE,        DIRECTION::NORTHEAST },
    { 'b',              DIRECTION::SOUTHWEST },
    { KEY_END,          DIRECTION::SOUTHWEST },
    { 'n',              DIRECTION::SOUTHEAST },
    { KEY_NPAGE,        DIRECTION::SOUTHEAST },
    { 0x1b,/* ESCAPE */ DIRECTION::CANCELLED },
},
itemmap_{
    { ITEMTYPE::NOTHING,        ' ' },
    { ITEMTYPE::BAT,            'B' },
    { ITEMTYPE::CUBE,           'C' },
    { ITEMTYPE::DRAGON,         'D' },
    { ITEMTYPE::FLOATINGEYE,    'F' },
    { ITEMTYPE::HOBGOBLIN,      'H' },
    { ITEMTYPE::KOBOLD,         'K' },
    { ITEMTYPE::LIZARDMAN,      'L' },
    { ITEMTYPE::MINOTAUR,       'M' },
    { ITEMTYPE::NAGA,           'N' },
    { ITEMTYPE::ORC,            'O' },
    { ITEMTYPE::RAT,            'R' },
    { ITEMTYPE::SPIDER,         'S' },
    { ITEMTYPE::TROLL,          'T' },
    { ITEMTYPE::WIZARD,         'W' },
    { ITEMTYPE::ZOMBIE,         'Z' },
    { ITEMTYPE::SHIELD,         ']' },
    { ITEMTYPE::WEAPON,         ')' },
    { ITEMTYPE::POTION,         '!' },
    { ITEMTYPE::KEY,            'k' },
},
tilemap_{},
lines_{0}, cols_{0}, messageWinWidth_{0}, lastTick_{ clock() },
titleText_{""}, messages_{} {
}

int View::ViewImpl::createTitleWin(WINDOW* win, int /* ncols */) {
    impl_.setTitleWin(win);

    return 0;
}

void View::ViewImpl::drawActors(WINDOW* viewport, World& world, int top,
int left) {
    mvwaddch(viewport, world.playerRow() - top, world.playerCol() - left,
        tilemap_[TERRAIN::PLAYER] | COLOR_PAIR(5) | A_BOLD);
}

void View::ViewImpl::drawInventory(Player& player) {
    auto inventory = inventory_.get();
    werase(inventory);
    mvwaddstr(inventory, 0, 5, "wielding");
    mvwaddstr(inventory, 0, 30, "carrying");
    mvwprintw(inventory, 0, 55, "stamina: %02d", player.health());
    int row = 1;
    int key = 1;
    player.foreach_wielded([&](ITEMPTR& item) {
        Item* temp = item.get();
        std::string name;
        if (temp == nullptr) {
            name = "nothing";
        } else {
            name = temp->article() + " " + temp->name();
        }
        mvwprintw(inventory, row++, 5, "%d %s", key++, name.c_str());
    });
    row = 1;
    player.foreach_carried([&](ITEMPTR& item) {
        Item* temp = item.get();
        std::string article, name;
        if (temp == nullptr) {
            name = "nothing";
        } else {
            name = temp->article() + " " + temp->name();
        }
        mvwprintw(inventory, row++, 30, "%d %s", key++, name.c_str());
    });
}

void View::ViewImpl::drawItems(WINDOW* viewport, World& world, int top,
int left, int height, int width) {
    world.foreach_item(top, left, height, width, [=](int row, int col, ITEMPTR& item) {
        chtype t;

        Tile* tile = world.tileAt(row, col);
        if (tile->visible() == false && tile->seen() == false) {
            t = tilemap_[TERRAIN::EMPTY];
            mvwaddch(viewport, row - top, col - left, t);
            return;
        }

        switch(item->type()) {
            case ITEMTYPE::DOOR: {
                    auto d = dynamic_cast<Door*>(item.get());
                    if (d->open()) {
                        t = (d->horizontal()) ? tilemap_[TERRAIN::H_DOOR_OPEN] | COLOR_PAIR(7)
                            : tilemap_[TERRAIN::V_DOOR_OPEN] | COLOR_PAIR(7);
                    } else {
                        t = (d->horizontal()) ? tilemap_[TERRAIN::H_DOOR_CLOSED] | COLOR_PAIR(7)
                            : tilemap_[TERRAIN::V_DOOR_CLOSED] | COLOR_PAIR(7);
                    }
                }
                break;
            case ITEMTYPE::TRAP: {
                    auto trap = dynamic_cast<Trap*>(item.get());
                    if (trap->sprung()) {
                        t = tilemap_[TERRAIN::TRAP]  | COLOR_PAIR(5);
                    } else {
                        t = tilemap_[TERRAIN::FLOOR] | COLOR_PAIR(2);
                    }

                }
                break;
            default: {
                    t = itemmap_[item->type()] |
                        (dynamic_cast<Monster*>(item.get()) ? COLOR_PAIR(6) : COLOR_PAIR(5));
                }
                break;
        }

        if (tile->visible()) {
            t |= A_BOLD;
        }
        mvwaddch(viewport, row - top, col - left, t);
    });
}

void View::ViewImpl::drawMessage() {
    auto message = message_.get();

    werase(message);
    for(auto & msg : messages_) {
        wprintw(message, "%s\n", msg.c_str());
    }
    wnoutrefresh(message);
}

void View::ViewImpl::drawTitle() {
    auto title = title_.get();

    werase(title);
    const int len = titleText_.length();
    mvwaddstr(title, 0, (impl_.cols_ - len)/2, titleText_.c_str());
    wnoutrefresh(title);
}

void View::ViewImpl::drawViewport(World &world) {
    auto viewport = viewport_.get();
    int screenHeight, screenWidth;
    getmaxyx(viewport, screenHeight, screenWidth);

    wbkgd(viewport, tilemap_[TERRAIN::EMPTY]);

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

            if (t->visible() == false && t->seen() == false) {
                display = tilemap_[TERRAIN::EMPTY];
                mvwaddch(viewport, row, col, display);
                continue;
            } else {
                display = tilemap_[t->terrain()];

                if (t->isBlock()) {
                    display |= COLOR_PAIR(2);
                }
            }
            if (t->visible()) {
                display |= A_BOLD;
            }
            mvwaddch(viewport, row, col, display);
        }
    }

    drawItems(viewport, world, top, left, screenHeight, screenWidth);

    drawActors(viewport, world, top, left);

    wnoutrefresh(viewport);
}

void View::ViewImpl::end_sig(int /* sig */) {
    View view;
    view.end();
}

bool View::ViewImpl::oneBeatPassed() {
    clock_t tick = clock();

    if ((tick - lastTick_) > (CLOCKS_PER_SEC / BEATS_PER_SECOND)) {
        lastTick_ = tick;
        return true;
    }
    return false;
}

void View::ViewImpl::setTitleWin(WINDOW*& win) {

    title_.reset(win);
}
