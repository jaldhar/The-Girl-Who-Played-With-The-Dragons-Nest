#include "item.h"

struct Item::ItemImpl {
    ItemImpl();
    ItemImpl(std::string article, std::string name, ITEMTYPE type);
    ~ItemImpl()=default;

    std::string article_;
    std::string name_;
    ITEMTYPE    type_;
};

Item::Item() : impl_ { new Item::ItemImpl() } {
}

Item::~Item() {

}

Item::Item(std::string article, std::string name, ITEMTYPE type) :
    impl_ { new Item::ItemImpl(article, name, type) } {
}

std::string Item::article() const {
    return impl_->article_;
}

void Item::setArticle(std::string article) {
    impl_->article_ = article;
}

std::string Item::name() const {
    return impl_->name_;
}

void Item::setName(std::string name) {
    impl_->name_ = name;
}

ITEMTYPE Item::type() const {
    return impl_->type_;
}

void Item::setType(ITEMTYPE type) {
    impl_->type_ = type;
}

Item::ItemImpl::ItemImpl() : ItemImpl("", "", ITEMTYPE::NOTHING) {
}

Item::ItemImpl::ItemImpl(std::string article, std::string name, ITEMTYPE type) :
article_{article},
name_{name},
type_{type} {
}
