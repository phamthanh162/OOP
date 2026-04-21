#include "Book.h"
#include <utility>

Book::Book(std::string id, std::string title, double basePrice, std::string author)
    : Item(std::move(id), std::move(title), basePrice), author(std::move(author)) {}

double Book::calculatePrice() const {
    // Giả sử sách thông thường giữ nguyên giá bìa
    return basePrice;
}

std::string Book::getAuthor() const {
    return author;
}