#include "BookstoreApp.h"

// Giữ lại đoạn này để phòng hờ terminal in log tiếng Việt không bị lỗi
#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    // Khởi chạy ứng dụng
    BookstoreApp myApp;
    myApp.run(); 
    
    return 0;
}