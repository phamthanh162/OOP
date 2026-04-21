#include "Item.h"
#include <utility> // Dùng cho std::move (tối ưu hiệu năng C++ hiện đại)

Item::Item(std::string id, std::string title, double basePrice)
    : id(std::move(id)), title(std::move(title)), basePrice(basePrice) {}

std::string Item::getTitle() const {
    return title;
}

std::string Item::getId() const {
    return id;
}

double Item::getBasePrice() const {
    return basePrice;
}