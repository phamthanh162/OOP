#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Item.h"

enum class CustomerTier {
    Normal = 100, // Trả 100%
    Vip = 90,     // Trả 90%
    Vvip = 80     // Trả 80%
};

class Transaction {
private:
    std::string customerPhone;
    CustomerTier customerTier;
    std::vector<std::pair<Item*, int>> cart; 

public:
    Transaction(std::string phone, CustomerTier tier = CustomerTier::Normal);

    void AddItemToCart(Item* item, int quantity);
    void UpdateQuantity(size_t index, int delta); // Tăng/giảm số lượng
    
    double CalculateFinalTotal() const;
    size_t GetCartSize() const;
    
    const std::vector<std::pair<Item*, int>>& GetCart() const { return cart; }
    void SetTier(CustomerTier tier) { customerTier = tier; } // Cập nhật hạng ngay lập tức
};