#include "Book.h"
#include <utility>

Book::Book(std::string id, std::string title, double basePrice, std::string author)
    : Item(std::move(id), std::move(title), basePrice), author(std::move(author)) {}

double Book::CalculateLineTotal(int quantity) const {
    // Giá giữ nguyên, chỉ nhân với số lượng
    return basePrice * quantity;
}

std::string Book::GetAuthor() const {
    return author;
}