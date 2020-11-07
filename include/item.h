#ifndef ITEM_H
#define ITEM_H

#include <memory>
#include <string>
#include "itemtype.h"

class Item
{
public:
    Item();
    Item(std::string article, std::string name, ITEMTYPE type);
     virtual ~Item();
     std::string article() const;
     void        setArticle(std::string article);
     std::string name() const;
     void        setName(std::string name);
     ITEMTYPE    type() const;
     void        setType(ITEMTYPE type);

 private:
    struct ItemImpl;
    std::unique_ptr<ItemImpl> impl_;
};

typedef std::unique_ptr<Item> ITEMPTR;

#endif // ITEM_H
