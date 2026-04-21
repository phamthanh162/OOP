#pragma once
#include <string>
#include "imgui.h"
#include <GLFW/glfw3.h>

class App {
protected:
    GLFWwindow* window;
    std::string title;
    int width, height;

    // Các hàm khởi tạo nội bộ
    bool initGLFW();
    bool initImGui();

public:
    App(std::string title, int width, int height);
    virtual ~App();

    // Hàm chạy ứng dụng (Template Method Pattern)
    void run();

    // Hàm ảo thuần túy: Các lớp con phải định nghĩa nội dung giao diện ở đây
    virtual void onRenderUI() = 0; 
};