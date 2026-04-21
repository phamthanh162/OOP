#include "Magazine.h"
#include <utility>

Magazine::Magazine(std::string id, std::string title, double basePrice, int issueNumber)
    : Item(std::move(id), std::move(title), basePrice), issueNumber(issueNumber) {}

double Magazine::calculatePrice() const {
    // Thể hiện tính đa hình: Tạp chí có cách tính giá khác, ví dụ giảm 10% so với giá bìa
    return basePrice * 0.9;
}

int Magazine::getIssueNumber() const {
    return issueNumber;
}