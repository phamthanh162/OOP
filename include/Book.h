#pragma once
#include "Item.h"
#include <string>

class Book : public Item {
private:
    std::string author;

public:
    Book(std::string id, std::string title, double basePrice, std::string author);

    // Ghi đè hàm tính giá của lớp cha (Thể hiện tính Đa hình)
    double calculatePrice() const override;

    std::string getAuthor() const;
};