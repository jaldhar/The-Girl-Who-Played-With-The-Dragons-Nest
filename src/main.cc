#include "game.h"

const char *name = "The Girl Who Played With The Dragons Nest";
const char *version = "1.2";

int main (int, char **) {
    Game game;

    return game.run(name, version);
}
