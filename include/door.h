#ifndef DOOR_H
#define DOOR_H

#include <memory>
#include "item.h"

class Door : public Item {
public:
    Door();
    virtual ~Door();
     bool horizontal() const;
     void setHorizontal(bool horizontal);
     bool open() const;
     void setOpen(bool open);

private:
     struct DoorImpl;
     std::unique_ptr<DoorImpl> impl_;
};

#endif // DOOR_H
