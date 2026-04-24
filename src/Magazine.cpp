#include "Magazine.h"
#include <utility>

Magazine::Magazine(std::string id, std::string title, double basePrice, int issueNumber)
    : Item(std::move(id), std::move(title), basePrice), issueNumber(issueNumber) {}

double Magazine::CalculateLineTotal(int quantity) const {
    double total = basePrice * quantity;
    // Luật đa hình riêng của Tạp chí
    if (quantity >= 5) {
        total *= 0.9; // Giảm 10%
    }
    return total;
}

int Magazine::GetIssueNumber() const {
    return issueNumber;
}