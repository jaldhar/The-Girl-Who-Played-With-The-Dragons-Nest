#ifndef VIEW_H
#define VIEW_H

#include <string>

#include "direction.h"
#include "game.h"
#include "player.h"
#include "world.h"
#include "state.h"

class View {
public:
    View()=default;
    ~View()=default;
    void  alert();
    STATE draw(World& world, Player& player);
    void  end();
    STATE handleTopLevelInput(Game* game);
    DIRECTION handleDirectionInput(Game* game);
    int   handleNumericalInput(Game* game);
    bool  handleBooleanInput(Game* game);
    void  init(std::string titleText);
    void  message(std::string msg);
    void  pause(Game* game);
    void  refresh();
    void  resize(World& world);
    void  shell();
private:
    struct ViewImpl;
    static ViewImpl _impl;
};

#endif // VIEW_H
