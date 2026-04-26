#pragma once
#include "Item.h"
#include <string>

class Book : public Item {
private:
    std::string author;

public:
    Book(std::string id, std::string title, double basePrice, std::string author);

    // Ghi đè hàm tính tiền. Giả sử Sách không giảm giá theo số lượng.
    double CalculateLineTotal(int quantity) const override;

    std::string GetAuthor() const;
};