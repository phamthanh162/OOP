#include "App.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>

App::App(std::string title, int width, int height) 
    : title(title), width(width), height(height), window(nullptr) {}

App::~App() {
    // Dọn dẹp bộ nhớ khi tắt app
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    if (window) glfwDestroyWindow(window);
    glfwTerminate();
}

bool App::initGLFW() {
    if (!glfwInit()) return false;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) return false;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Bật V-Sync cho mượt khung hình
    return true;
}

bool App::initImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGuiIO& io = ImGui::GetIO(); (void)io; // Lấy đối tượng IO của ImGui
    
    // TẮT TÍNH NĂNG LƯU TRẠNG THÁI CỬA SỔ
    io.IniFilename = nullptr;
    
    // --- BẮT ĐẦU NẠP FONT TIẾNG VIỆT ---
    // Đường dẫn trỏ thẳng vào font Arial mặc định của mọi máy Windows
    ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesVietnamese());
    
    if (font == nullptr) {
        std::cerr << "CẢNH BÁO: Không thể tải font tiếng Việt. Đang dùng font mặc định!\n";
    }
    // --- KẾT THÚC NẠP FONT ---

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    return true;
}

void App::run() {
    if (!initGLFW() || !initImGui()) return;

    // Vòng lặp chính của ứng dụng
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Nơi gọi giao diện từ các lớp con kế thừa App (như BookstoreApp)
        onRenderUI(); 

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Màu xám đen viền ngoài
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
}