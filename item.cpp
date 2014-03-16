#include "item.h"

using namespace std;

struct Item::ItemImpl {
    ItemImpl();
    ItemImpl(string article, string name, ITEMTYPE type);
    ~ItemImpl()=default;

    string      _article;
    string      _name;
    ITEMTYPE    _type;
};

Item::Item() : _impl { new Item::ItemImpl() } {
}

Item::~Item() {

}

Item::Item(string article, string name, ITEMTYPE type) :
    _impl { new Item::ItemImpl(article, name, type) } {
}

string Item::article() const {
    return _impl->_article;
}

void Item::setArticle(string article) {
    _impl->_article = article;
}

string Item::name() const {
    return _impl->_name;
}

void Item::setName(string name) {
    _impl->_name = name;
}

ITEMTYPE Item::type() const {
    return _impl->_type;
}

void Item::setType(ITEMTYPE type) {
    _impl->_type = type;
}

Item::ItemImpl::ItemImpl() {
    ItemImpl("", "", ITEMTYPE::NOTHING);
}

Item::ItemImpl::ItemImpl(string article, string name, ITEMTYPE type) {
    _article = article;
    _name = name;
    _type = type;
}
