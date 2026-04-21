#pragma once
#include <string>

class Item {
protected:
    std::string id;
    std::string title;
    double basePrice;

public:
    Item(std::string id, std::string title, double basePrice);
    
    // Virtual destructor: Bắt buộc phải có để giải phóng bộ nhớ đúng cách khi dùng Đa hình
    virtual ~Item() = default;

    // Hàm thuần ảo (Pure virtual function): Bắt buộc các lớp con phải tự định nghĩa cách tính giá
    virtual double calculatePrice() const = 0;

    // Các hàm Getters cơ bản
    std::string getTitle() const;
    std::string getId() const;
    double getBasePrice() const;
};