#ifndef PLAYER_H
#define PLAYER_H

#include <functional>
#include "combat.h"
#include "item.h"

class Player : public Combat {
public:
    Player();
    ~Player()=default;
    int                      facingX() const;
    void                     setFacingX(int x);
    int                      facingY() const;
    void                     setFacingY(int y);
    bool                     keepFighting() const;
    void                     setKeepFighting(bool fight);
    bool                     keepMoving() const;
    void                     setKeepMoving(bool move);
    bool                     pickup() const;
    void                     setPickup(bool pickup);
    bool                     carry(Item* item);
    bool                     wield(Item* item);
    Item*                    drop(int dropped);
    void                     foreach_carried(std::function<void(std::unique_ptr<Item>&)> callback);
    void                     foreach_wielded(std::function<void(std::unique_ptr<Item>&)> callback);
private:
    struct PlayerImpl;
    static PlayerImpl impl_;
};

#endif // PLAYER_H
