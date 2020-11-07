#ifndef TRAP_H
#define TRAP_H

#include <memory>
#include "item.h"

class Trap : public Item
{
public:
    Trap();
    virtual ~Trap();
    bool sprung() const;
    void setSprung(bool spring);
private:
     struct TrapImpl;
     std::unique_ptr<TrapImpl> impl_;
};

#endif // TRAP_H
