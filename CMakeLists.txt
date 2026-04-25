cmake_minimum_required(VERSION 3.14)
project(BookstoreManagement)

# Chuẩn của dự án - C++ 26
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Bật tính năng tự động tải thư viện của CMake
include(FetchContent)

# ---------------------------------------------------------
# 1. TẢI GLFW (Công cụ tạo cửa sổ ứng dụng)
# ---------------------------------------------------------
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
# Tắt bớt các module không cần thiết của GLFW cho nhẹ
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# ---------------------------------------------------------
# 2. TẢI DEAR IMGUI
# ---------------------------------------------------------
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG master 
)
FetchContent_MakeAvailable(imgui)
FetchContent_GetProperties(imgui SOURCE_DIR IMGUI_DIR)

# Gom các file mã nguồn lõi của ImGui
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    # Lấy backend cho GLFW và OpenGL3
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# ---------------------------------------------------------
# 3. CẤU HÌNH DỰ ÁN CỦA NHÓM
# ---------------------------------------------------------
include_directories(include)
set(SOURCES
    src/main.cpp
    src/BookstoreApp.cpp
    src/Khohang.cpp
    src/LichSuHoaDon.cpp
)

# Tạo phần mềm từ code của nhóm + code của ImGui
add_executable(BookstoreManagement ${SOURCES} ${IMGUI_SOURCES})

# Trỏ đường dẫn để g++ hiểu lệnh #include "imgui.h"
target_include_directories(BookstoreManagement PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# Tìm thư viện đồ họa OpenGL có sẵn trên máy (Windows/Mac/Linux đều có)
find_package(OpenGL REQUIRED)

# Ghép nối tất cả lại với nhau
target_link_libraries(BookstoreManagement PRIVATE glfw OpenGL::GL)