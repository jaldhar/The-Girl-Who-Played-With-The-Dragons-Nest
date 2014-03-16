#include "game.h"
using namespace std;

const char *name = "The Girl Who Played With The Dragons Nest";
const char *version = "1.0";

int main (int, char **) {
    Game game;

    return game.run(name, version);
}
