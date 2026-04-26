/*
  BookstoreApp.cpp - Giao dien ImGui
  Su dung: QuanLyKho (kho) + LichSuHoaDon (lichSu)
*/
#include "BookstoreApp.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <format>
#include <ranges>
#include <algorithm>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <cmath>
using namespace std;

/* ================================================================ MAU SAC */
static const ImVec4 C_XANH  = {0.18f,0.80f,0.44f,1.f};
static const ImVec4 C_VANG  = {0.95f,0.77f,0.06f,1.f};
static const ImVec4 C_DO    = {0.91f,0.30f,0.24f,1.f};
static const ImVec4 C_TIM   = {0.61f,0.35f,0.71f,1.f};
static const ImVec4 C_DUONG = {0.20f,0.60f,0.86f,1.f};
static const ImVec4 C_CAM   = {0.90f,0.50f,0.10f,1.f};
static const ImVec4 C_XAM   = {0.55f,0.55f,0.55f,1.f};

static ImVec4 mauLoai(LoaiHang l) {
    switch (l) {
        case LoaiHang::Sach:        return C_XANH;
        case LoaiHang::VoPhanViet:  return C_DUONG;
        case LoaiHang::DoChoi:      return C_VANG;
        case LoaiHang::LuuNiem:     return C_TIM;
        default:                    return C_XAM;
    }
}

static float lamTron(float x) { return (float)(floor(x/1000.0+0.5)*1000.0); }

static void layNgayGio(char* ngay, char* gio) {
    time_t t = time(nullptr);
    auto* ti = localtime(&t);
    strftime(ngay,12,"%d/%m/%Y",ti);
    strftime(gio,9,"%H:%M:%S",ti);
}

/* ================================================================ CONSTRUCTOR */
BookstoreApp::BookstoreApp() {
    kho.khoiTao();
    lichSu.khoiTao();
}

/* ================================================================
   TRANG: MENU CHINH
================================================================ */
void BookstoreApp::renderMenuChinh() {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos({0,0});
    ImGui::SetNextWindowSize(io.DisplaySize);

    ImGui::Begin("##menu", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus);

    float W = io.DisplaySize.x;

    auto centerText = [&](const char* txt, ImVec4 col) {
        ImGui::SetCursorPosX((W - ImGui::CalcTextSize(txt).x) * 0.5f);
        ImGui::TextColored(col, "%s", txt);
    };

    ImGui::Spacing();
    centerText("NHA SACH FETEL", C_VANG);
    centerText("HCMUS - 23DTV_CLC1 | 235 Nguyen Van Cu | 23207022.19.14", C_XAM);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (!thongBao.empty()) {
        ImGui::SetCursorPosX(20);
        ImGui::TextColored(C_XANH, "%s", thongBao.c_str());
        ImGui::SameLine();
        if (ImGui::SmallButton("x")) thongBao = "";
        ImGui::Spacing();
    }

    int tongHang = (int)kho.kho.size();
    int hetHang  = (int)ranges::count_if(kho.kho, [](const auto& h){return h->soLuongTon==0;});
    int sapHet   = (int)ranges::count_if(kho.kho, [](const auto& h){return h->soLuongTon<5 && !h->ngungBan;});

    ImGui::SetCursorPosX(20);
    ImGui::TextColored(C_XAM, "Hang hoa: ");
    ImGui::SameLine();
    ImGui::TextColored(C_VANG, "%d", tongHang);

    ImGui::SameLine(0,30);
    ImGui::TextColored(C_XAM, "Hoa don: ");
    ImGui::SameLine();
    ImGui::TextColored(C_VANG, "%d", (int)lichSu.dsHoaDon.size());

    ImGui::SameLine(0,30);
    ImGui::TextColored(C_XAM, "Khach than thiet: ");
    ImGui::SameLine();
    ImGui::TextColored(C_VANG, "%d", (int)lichSu.dsKH.size());

    if (hetHang > 0) {
        ImGui::TextColored(C_DO, "[!] Het hang: %d", hetHang);
        ImGui::SameLine(0,15);
    }
    if (sapHet > 0) {
        ImGui::TextColored(C_CAM, "[!] Sap het: %d", sapHet);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    float btnW=210, btnH=80, gap=20;
    float totalW = 5*btnW + 4*gap;
    float startX = (W - totalW) * 0.5f;

    auto bigBtn = [&](const char* label, ImVec4 col, float x, float y) -> bool {
        ImGui::SetCursorPos({x, y});
        ImGui::PushStyleColor(ImGuiCol_Button, col);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {col.x+0.08f,col.y+0.08f,col.z+0.08f,1.f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, {col.x-0.08f,col.y-0.08f,col.z-0.08f,1.f});

        bool r = ImGui::Button(label, {btnW, btnH});

        ImGui::PopStyleColor(3);
        return r;
    };

    float rowY = 170;

    if (bigBtn("BAN HANG", {0.13f,0.62f,0.38f,1.f}, startX, rowY))
        showBanHang = true;

    if (bigBtn("QUAN LY KHO", {0.13f,0.45f,0.72f,1.f}, startX+(btnW+gap), rowY))
        showKho = true;

    if (bigBtn("LICH SU HOA DON", {0.50f,0.25f,0.60f,1.f}, startX+2*(btnW+gap), rowY)) {
        showLichSu = true;
        hdDangXem = nullptr;
    }

    if (bigBtn("KHACH HANG", {0.70f,0.40f,0.10f,1.f}, startX+3*(btnW+gap), rowY))
        showKhachHang = true;

    if (bigBtn("BAO CAO", {0.50f,0.18f,0.18f,1.f}, startX+4*(btnW+gap), rowY))
        showBaoCao = true;

    ImGui::End();
}
void BookstoreApp::renderQuanLyKho() {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos({0,0});
    ImGui::SetNextWindowSize(io.DisplaySize);

    ImGui::Begin("##kho", nullptr,
        ImGuiWindowFlags_NoTitleBar|
        ImGuiWindowFlags_NoResize|
        ImGuiWindowFlags_NoMove);

    if (ImGui::Button("< Menu")) showKho = false;

    ImGui::SameLine(0,20);
    ImGui::TextColored(C_VANG, "QUAN LY KHO HANG");

    ImGui::SameLine(0,30);
    ImGui::TextColored(C_XAM, "Tim:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(220);
    ImGui::InputText("##timkho", buf_tim, sizeof(buf_tim));

    ImGui::SameLine(0,15);
    if (ImGui::Button("Them hang moi")) {
        resetBuf();
        showFormThem = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("Lam moi")) kho.khoiTao();

    ImGui::Separator();
    ImGui::Spacing();

    float tableH = io.DisplaySize.y - 130;

    if (ImGui::BeginTable("tblKho", 7,
        ImGuiTableFlags_Borders|
        ImGuiTableFlags_RowBg|
        ImGuiTableFlags_ScrollY|
        ImGuiTableFlags_Resizable, {0,tableH}))
    {
        ImGui::TableSetupScrollFreeze(0,1);
        ImGui::TableSetupColumn("Ma", ImGuiTableColumnFlags_WidthFixed, 55);
        ImGui::TableSetupColumn("Ten hang", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Loai", ImGuiTableColumnFlags_WidthFixed, 110);
        ImGui::TableSetupColumn("NCC", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Gia ban", ImGuiTableColumnFlags_WidthFixed, 90);
        ImGui::TableSetupColumn("Ton kho", ImGuiTableColumnFlags_WidthFixed, 70);
        ImGui::TableSetupColumn("Hanh dong", ImGuiTableColumnFlags_WidthFixed, 130);
        ImGui::TableHeadersRow();

        string filter = buf_tim;

        for (auto& h : kho.kho) {
            if (!filter.empty()) {
                bool ok = h->ten.find(filter)!=string::npos ||
                          to_string(h->ma).find(filter)!=string::npos;
                if (!ok) continue;
            }

            ImGui::TableNextRow();

            if (h->soLuongTon < 5 && !h->ngungBan)
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,
                    ImGui::ColorConvertFloat4ToU32({0.4f,0.1f,0.1f,0.35f}));

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%d", h->ma);

            ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(mauLoai(h->loai), "%s", h->ten.c_str());

            ImGui::TableSetColumnIndex(2);
            ImGui::TextColored(mauLoai(h->loai), "%s", h->tenLoai().c_str());

            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%s", h->nhaCungCap.c_str());

            ImGui::TableSetColumnIndex(4);
            ImGui::Text("%.0f", h->giaBan);

            ImGui::TableSetColumnIndex(5);
            if (h->soLuongTon < 5)
                ImGui::TextColored(C_DO, "%d", h->soLuongTon);
            else
                ImGui::Text("%d", h->soLuongTon);

            ImGui::TableSetColumnIndex(6);
            ImGui::PushID(h->ma);

            if (ImGui::SmallButton("Nhap them")) {
                hangDangSua = h.get();
                snprintf(buf_sl, sizeof(buf_sl), "0");
                showFormNhap = true;
            }

            ImGui::SameLine();

            if (h->ngungBan) {
                if (ImGui::SmallButton("Mo ban")) {
                    kho.doiTrangThai(h->ma,false);
                    kho.luu();
                }
            } else {
                if (ImGui::SmallButton("Ngung")) {
                    kho.doiTrangThai(h->ma,true);
                    kho.luu();
                }
            }

            ImGui::PopID();
        }

        ImGui::EndTable();
    }

    ImGui::Separator();

    int het = (int)ranges::count_if(kho.kho,[](const auto&h){return h->soLuongTon==0;});
    int sap = (int)ranges::count_if(kho.kho,[](const auto&h){return h->soLuongTon<5&&!h->ngungBan;});

    ImGui::Text("Tong: %d mat hang", (int)kho.kho.size());
    ImGui::SameLine(200);
    ImGui::TextColored(C_DO, "Het hang: %d", het);

    ImGui::SameLine(320);
    ImGui::TextColored(C_CAM, "Sap het(<5): %d", sap);

    ImGui::End();
}
void BookstoreApp::renderBanHang() {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos({0,0});
    ImGui::SetNextWindowSize(io.DisplaySize);

    ImGui::Begin("##banhang",nullptr,
        ImGuiWindowFlags_NoTitleBar|
        ImGuiWindowFlags_NoResize|
        ImGuiWindowFlags_NoMove);

    if(ImGui::Button("< Menu")) showBanHang=false;

    ImGui::SameLine(0,20);
    ImGui::TextColored(C_XANH,"BAN HANG - TAO HOA DON");

    ImGui::Separator();

    /* ===== CỘT TRÁI ===== */
    ImGui::BeginChild("col_hang",{430,io.DisplaySize.y-80},true);

    ImGui::Text("CHON HANG");
    ImGui::Separator();

    ImGui::SetNextItemWidth(200);
    ImGui::InputText("Tim##bh",buf_tim,sizeof(buf_tim));

    if(ImGui::BeginTable("tblBH",4,
        ImGuiTableFlags_Borders|
        ImGuiTableFlags_RowBg|
        ImGuiTableFlags_ScrollY,
        {0,io.DisplaySize.y-170}))
    {
        ImGui::TableSetupScrollFreeze(0,1);
        ImGui::TableSetupColumn("Ma",ImGuiTableColumnFlags_WidthFixed,45);
        ImGui::TableSetupColumn("Ten",ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Gia",ImGuiTableColumnFlags_WidthFixed,80);
        ImGui::TableSetupColumn("Con",ImGuiTableColumnFlags_WidthFixed,40);
        ImGui::TableHeadersRow();

        string filter=buf_tim;

        for(auto&h:kho.kho){
            if(h->ngungBan||h->soLuongTon<=0) continue;

            if(!filter.empty() &&
               h->ten.find(filter)==string::npos &&
               to_string(h->ma).find(filter)==string::npos)
                continue;

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%d",h->ma);

            ImGui::TableSetColumnIndex(1);
            ImGui::PushID(h->ma);
            if(ImGui::Selectable(h->ten.c_str(),false,ImGuiSelectableFlags_SpanAllColumns)){
                hangDangChon=h.get();
                snprintf(buf_sl,sizeof(buf_sl),"1");
            }
            ImGui::PopID();

            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%.0f",h->giaBan);

            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%d",h->soLuongTon);
        }

        ImGui::EndTable();
    }

    ImGui::EndChild();

    ImGui::SameLine();

    /* ===== CỘT PHẢI ===== */
    ImGui::BeginChild("col_hd",{0,io.DisplaySize.y-80},true);

    ImGui::Text("HOA DON");
    ImGui::Separator();

    ImGui::SetNextItemWidth(180);
    ImGui::InputText("SDT Khach",buf_sdt,sizeof(buf_sdt));

    ImGui::SetNextItemWidth(240);
    ImGui::InputText("Ten khach",buf_ten_kh,sizeof(buf_ten_kh));

    /* ===== THÊM MẶT HÀNG ===== */
    ImGui::Separator();

    if(hangDangChon){
        ImGui::TextColored(C_XANH,"Chon: %s",
            hangDangChon->ten.c_str());

        ImGui::SetNextItemWidth(70);
        ImGui::InputText("SL##bh",buf_sl,sizeof(buf_sl));

        ImGui::SameLine();

        if(ImGui::Button("Them vao HD")){
            int sl=atoi(buf_sl);

            if(sl>0 && sl<=hangDangChon->soLuongTon){
                auto&ct=hdChiTiet[hdSoMatHang++];

                ct.maHang=hangDangChon->ma;
                ct.soLuong=sl;
                ct.giaBan=hangDangChon->giaBan;
                ct.tenHang=hangDangChon->ten;
                ct.thanhTien=hangDangChon->giaBan*sl;

                hdTongTruocGiam+=ct.thanhTien;

                hangDangChon->soLuongTon-=sl;
                hangDangChon=nullptr;
            }
        }

        ImGui::SameLine();
        if(ImGui::Button("Bo chon")) hangDangChon=nullptr;
    }

    /* ===== DANH SÁCH HÓA ĐƠN ===== */
    ImGui::Separator();

    if(ImGui::BeginTable("tblCT",4,
        ImGuiTableFlags_Borders|
        ImGuiTableFlags_RowBg|
        ImGuiTableFlags_ScrollY,
        {0,180}))
    {
        ImGui::TableSetupColumn("Ten");
        ImGui::TableSetupColumn("SL",ImGuiTableColumnFlags_WidthFixed,40);
        ImGui::TableSetupColumn("Gia",ImGuiTableColumnFlags_WidthFixed,80);
        ImGui::TableSetupColumn("T.Tien",ImGuiTableColumnFlags_WidthFixed,100);
        ImGui::TableHeadersRow();

        for(int i=0;i<hdSoMatHang;i++){
            auto&ct=hdChiTiet[i];

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s",ct.tenHang.c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%d",ct.soLuong);

            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%.0f",ct.giaBan);

            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%.0f",ct.thanhTien);
        }

        ImGui::EndTable();
    }

    /* ===== TÍNH TIỀN ===== */
    ImGui::Separator();

    ImGui::TextColored(C_VANG,"THANH TOAN: %.0f VND",hdTongTruocGiam);

    if(ImGui::Button("XUAT HOA DON")){
        hdSoMatHang=0;
        hdTongTruocGiam=0;
        memset(buf_sdt,0,sizeof(buf_sdt));
        memset(buf_ten_kh,0,sizeof(buf_ten_kh));
    }

    ImGui::SameLine();

    if(ImGui::Button("Huy HD")){
        hdSoMatHang=0;
        hdTongTruocGiam=0;
        hangDangChon=nullptr;
    }

    ImGui::EndChild();
    ImGui::End();
}
/* ================================================================
   TRANG: LICH SU HOA DON
================================================================ */
void BookstoreApp::renderLichSuHoaDon() {
    ImGui::Begin("Lich su hoa don");

    ImGui::Text("Chuc nang dang phat trien...");

    ImGui::End();
}

/* ================================================================
   TRANG: KHACH HANG
================================================================ */
void BookstoreApp::renderKhachHang() {
    ImGui::Begin("Khach hang");

    ImGui::Text("Chuc nang dang phat trien...");

    ImGui::End();
}

/* ================================================================
   RESET BUFFER
================================================================ */
void BookstoreApp::resetBuf() {
    memset(buf_ten,0,sizeof(buf_ten));
    memset(buf_ncc,0,sizeof(buf_ncc));
    memset(buf_gia_nhap,0,sizeof(buf_gia_nhap));
    memset(buf_gia_ban,0,sizeof(buf_gia_ban));
    memset(buf_sl,0,sizeof(buf_sl));
    memset(buf_extra1,0,sizeof(buf_extra1));
    memset(buf_extra2,0,sizeof(buf_extra2));
    memset(buf_sdt,0,sizeof(buf_sdt));
    memset(buf_ten_kh,0,sizeof(buf_ten_kh));
}

/* ================================================================
   MAIN LOOP
================================================================ */
void BookstoreApp::run() {
    if(!glfwInit()){
        printf("GLFW init failed\n");
        return;
    }

    GLFWwindow* window = glfwCreateWindow(1280,720,"Bookstore",NULL,NULL);
    glfwMakeContextCurrent(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window,true);
    ImGui_ImplOpenGL3_Init("#version 330");

    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        renderMenuChinh();

        if(showBanHang)   renderBanHang();
        if(showKho)       renderQuanLyKho();
        if(showLichSu)    renderLichSuHoaDon();
        if(showKhachHang) renderKhachHang();

        ImGui::Render();

        int w,h;
        glfwGetFramebufferSize(window,&w,&h);
        glViewport(0,0,w,h);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}