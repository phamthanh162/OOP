#pragma once
#include "Item.h"

class Magazine : public Item {
private:
    int issueNumber;

public:
    Magazine(std::string id, std::string title, double basePrice, int issueNumber);

    // Ghi đè hàm tính tiền. Thể hiện Đa hình: Tạp chí mua từ 5 cuốn trở lên giảm 10%
    double CalculateLineTotal(int quantity) const override;

    int GetIssueNumber() const;
};