#include "Transaction.h"
#include <utility>

Transaction::Transaction(std::string phone, CustomerTier tier)
    : customerPhone(std::move(phone)), customerTier(tier) {}

void Transaction::AddItemToCart(Item* item, int quantity) {
    if (item == nullptr || quantity <= 0) return;

    // Tự động gộp số lượng nếu món hàng đã có trong giỏ
    for (auto& cartItem : cart) {
        if (cartItem.first->GetId() == item->GetId()) {
            cartItem.second += quantity; 
            return; 
        }
    }
    cart.push_back({item, quantity});
}

void Transaction::UpdateQuantity(size_t index, int delta) {
    if (index < cart.size()) {
        cart[index].second += delta;
        // Xóa món hàng nếu số lượng <= 0
        if (cart[index].second <= 0) {
            cart.erase(cart.begin() + index);
        }
    }
}

double Transaction::CalculateFinalTotal() const {
    double subTotal = 0.0;
    for (const auto& cartItem : cart) {
        subTotal += cartItem.first->CalculateLineTotal(cartItem.second); 
    }
    return subTotal * (static_cast<double>(customerTier) / 100.0);
}

size_t Transaction::GetCartSize() const {
    return cart.size();
}