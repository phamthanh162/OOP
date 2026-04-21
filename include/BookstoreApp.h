#include "App.h"
#include <vector>
#include <memory>
#include "Item.h" // Class cha của các ấn phẩm

class BookstoreApp : public App {
private:
    std::vector<std::unique_ptr<Item>> inventory; // Danh sách đa hình

public:
    BookstoreApp() : App("Quan Ly Nha Sach v2.0", 1280, 720) {}

    // Ghi đè hàm vẽ giao diện
    void onRenderUI() override {
        ImGui::Begin("Dashboard");
        ImGui::Text("So luong an pham: %zu", inventory.size());
        
        if (ImGui::Button("Them Sach Moi")) {
            // Logic add polymorphism ở đây
        }

        ImGui::End();
    }
};