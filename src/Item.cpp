#include "Item.h"
#include <utility>

Item::Item(std::string id, std::string title, double basePrice)
    : id(std::move(id)), title(std::move(title)), basePrice(basePrice) {}

std::string Item::GetTitle() const {
    return title;
}

std::string Item::GetId() const {
    return id;
}

double Item::GetBasePrice() const {
    return basePrice;
}