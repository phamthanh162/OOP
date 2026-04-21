#pragma once
#include "Item.h"

class Magazine : public Item {
private:
    int issueNumber; // Số báo xuất bản

public:
    Magazine(std::string id, std::string title, double basePrice, int issueNumber);

    // Ghi đè hàm tính giá của lớp cha (Thể hiện tính Đa hình)
    double calculatePrice() const override;

    int getIssueNumber() const;
};