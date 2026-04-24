#include "BookstoreApp.h"
#include "Book.h"
#include "Magazine.h"
#include <imgui.h>
#include <fstream>
#include <format>

BookstoreApp::BookstoreApp() : App("NHÀ SÁCH FETEL - HỆ THỐNG QUẢN LÝ", 1280, 720) {
    InitializeSampleData();
    currentSale = std::make_unique<Transaction>("");
}

void BookstoreApp::InitializeSampleData() {
    inventory.push_back(std::make_unique<Book>("B01", "Đắc Nhân Tâm", 100000, "Dale Carnegie"));
    inventory.push_back(std::make_unique<Book>("B02", "Sạch Code (Clean Code)", 250000, "Robert C. Martin"));
    inventory.push_back(std::make_unique<Magazine>("M01", "Tạp chí PC World", 50000, 125));
}

Item* BookstoreApp::FindItemById(const std::string& id) {
    for (auto& item : inventory) {
        if (item->GetId() == id) return item.get();
    }
    return nullptr;
}

void BookstoreApp::onRenderUI() {
    // Ép UI full màn hình, không cho di chuyển/thu nhỏ
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f)); 
    ImGui::SetNextWindowSize(io.DisplaySize);    
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;

    if (ImGui::Begin("MainWindow", nullptr, windowFlags)) {
        if (ImGui::BeginTabBar("MainTabBar")) {
            if (ImGui::BeginTabItem("1. BÁN HÀNG")) { DrawSellingTab(); ImGui::EndTabItem(); }
            if (ImGui::BeginTabItem("2. QUẢN LÝ KHO")) { DrawInventoryTab(); ImGui::EndTabItem(); }
            if (ImGui::BeginTabItem("3. KHÁCH HÀNG")) { DrawCustomerTab(); ImGui::EndTabItem(); }
            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}

void BookstoreApp::DrawSellingTab() {
    ImGui::Columns(2, "SellingColumns"); 

    // --- CỘT TRÁI ---
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "THÔNG TIN GIAO DỊCH");
    ImGui::Separator();
    
    ImGui::Checkbox("Khách vãng lai (Không lưu thông tin)", &isGuest);

    if (isGuest) ImGui::BeginDisabled();
    
    ImGui::InputText("SĐT Khách hàng", customerPhoneBuffer, IM_ARRAYSIZE(customerPhoneBuffer));
    const char* tiers[] = { "Thường (Normal)", "Thân thiết (VIP)", "Đặc biệt (VVIP)" };
    // Mảng ánh xạ giá trị thực tế tương ứng với 3 vị trí trên
    const CustomerTier tierValues[] = { CustomerTier::Normal, CustomerTier::Vip, CustomerTier::Vvip };

    if (ImGui::Combo("Hạng thành viên", &selectedTierIndex, tiers, IM_ARRAYSIZE(tiers))) {
        // Dùng mảng để lấy đúng giá trị (100, 90, 80) thay vì ép thẳng số (0, 1, 2)
        currentSale->SetTier(tierValues[selectedTierIndex]);
    }

    if (isGuest) {
        ImGui::EndDisabled();
        selectedTierIndex = 0; 
        currentSale->SetTier(CustomerTier::Normal); 
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("QUÉT MÃ HÀNG HÓA");
    ImGui::InputText("Mã sản phẩm", searchIdBuffer, IM_ARRAYSIZE(searchIdBuffer));

    if (ImGui::Button("Thêm vào giỏ", ImVec2(120, 40))) {
        Item* found = FindItemById(searchIdBuffer);
        if (found) {
            currentSale->AddItemToCart(found, 1);
            searchIdBuffer[0] = '\0'; 
        }
    }

    ImGui::NextColumn(); 

    // --- CỘT PHẢI ---
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "CHI TIẾT HÓA ĐƠN");
    ImGui::Separator();

    if (ImGui::BeginTable("CartTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Tên sản phẩm");
        ImGui::TableSetupColumn("SL", ImGuiTableColumnFlags_WidthFixed, 85.0f); 
        ImGui::TableSetupColumn("Đơn giá");
        ImGui::TableSetupColumn("Thành tiền");
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < currentSale->GetCart().size(); ++i) {
            Item* item = currentSale->GetCart()[i].first;
            int qty = currentSale->GetCart()[i].second;

            ImGui::TableNextRow();
            ImGui::TableNextColumn(); ImGui::Text("%s", item->GetTitle().c_str());
            
            ImGui::TableNextColumn(); 
            ImGui::PushID(i); 
            if (ImGui::Button("-", ImVec2(24, 0))) currentSale->UpdateQuantity(i, -1);
            ImGui::SameLine(); 
            ImGui::Text("%d", qty);
            ImGui::SameLine();
            if (ImGui::Button("+", ImVec2(24, 0))) currentSale->UpdateQuantity(i, 1);
            ImGui::PopID();

            ImGui::TableNextColumn(); ImGui::Text("%.0f", item->GetBasePrice());
            ImGui::TableNextColumn(); ImGui::Text("%.0f", item->CalculateLineTotal(qty)); 
        }
        ImGui::EndTable();
    }

    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Tổng cộng: %.0f VND", currentSale->CalculateFinalTotal());
    
    ImGui::Spacing();
    
    // Xác thực SĐT
    bool isPhoneValid = isGuest || (strlen(customerPhoneBuffer) > 0);
    bool isCartEmpty = (currentSale->GetCartSize() == 0);

    if (!isPhoneValid) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "(!) Vui lòng nhập SĐT hoặc chọn 'Khách vãng lai'");
    }

    ImGui::BeginDisabled(!isPhoneValid || isCartEmpty);
    if (ImGui::Button("XÁC NHẬN THANH TOÁN & IN BIÊN LAI", ImVec2(-1, 50))) {
        PrintReceipt(); 
    }
    ImGui::EndDisabled();

    ImGui::Columns(1); // Kết thúc chia cột
}

void BookstoreApp::PrintReceipt() {
    if (currentSale->GetCartSize() == 0) return; 

    std::ofstream file("HoaDon_Tam.txt");
    if (file.is_open()) {
        const char utf8_bom[] = {(char)0xEF, (char)0xBB, (char)0xBF};
        file.write(utf8_bom, 3);

        file << "======================================\n";
        file << "            NHÀ SÁCH FETEL            \n";
        file << "======================================\n";
        
        if (isGuest) file << "SĐT Khách hàng: KHÁCH VÃNG LAI\n";
        else file << "SĐT Khách hàng: " << customerPhoneBuffer << "\n";
        
        file << "--------------------------------------\n";
        
        for (const auto& cartItem : currentSale->GetCart()) {
            Item* item = cartItem.first;
            int qty = cartItem.second;
            file << std::format("{:<20} x{} : {:>10.0f} VND\n", item->GetTitle(), qty, item->CalculateLineTotal(qty));
        }
        
        file << "--------------------------------------\n";
        file << std::format("TỔNG CỘNG: {:>21.0f} VND\n", currentSale->CalculateFinalTotal());
        file << "======================================\n";
        file.close();

        // Reset sau khi thanh toán
        currentSale = std::make_unique<Transaction>(customerPhoneBuffer, static_cast<CustomerTier>(selectedTierIndex));
        searchIdBuffer[0] = '\0';
        if (isGuest) {
            customerPhoneBuffer[0] = '\0'; // Xóa SĐT cũ nếu có
        }

        // Cập nhật lại logic mảng
        const CustomerTier tierValues[] = { CustomerTier::Normal, CustomerTier::Vip, CustomerTier::Vvip };
        currentSale = std::make_unique<Transaction>(customerPhoneBuffer, tierValues[selectedTierIndex]);
    }
}

void BookstoreApp::DrawInventoryTab() { ImGui::Text("Chức năng đang phát triển..."); }
void BookstoreApp::DrawCustomerTab() { ImGui::Text("Chức năng đang phát triển..."); }