#pragma once
#include "App.h"
#include "Transaction.h"
#include "Item.h"
#include <vector>
#include <memory>
#include <string>

class BookstoreApp : public App {
private:
    std::vector<std::unique_ptr<Item>> inventory; 
    std::unique_ptr<Transaction> currentSale;      

    // Trạng thái giao diện
    char searchIdBuffer[64] = "";        
    char customerPhoneBuffer[64] = "";   
    char customerNameBuffer[128] = "";   
    int selectedTierIndex = 0;           
    bool isGuest = false; // Checkbox Khách vãng lai

    void DrawSellingTab();    
    void DrawInventoryTab();  
    void DrawCustomerTab();   
    void PrintReceipt();      

    Item* FindItemById(const std::string& id);

public:
    BookstoreApp();
    ~BookstoreApp() override = default;

    void onRenderUI() override; // Chữ 'o' viết thường để khớp với App.h
    void InitializeSampleData();
};