#pragma once
#include <string>

class Item {
protected:
    std::string id;
    std::string title;
    double basePrice;

public:
    Item(std::string id, std::string title, double basePrice);
    virtual ~Item() = default;

    // Truyền thêm số lượng (quantity) để lớp con tự quyết định việc giảm giá
    virtual double CalculateLineTotal(int quantity) const = 0;

    std::string GetTitle() const;
    std::string GetId() const;
    double GetBasePrice() const;
};