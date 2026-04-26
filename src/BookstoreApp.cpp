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
    time_t t = time(nullptr); auto* ti = localtime(&t);
    strftime(ngay,12,"%d/%m/%Y",ti); strftime(gio,9,"%H:%M:%S",ti);
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
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove     | ImGuiWindowFlags_NoBringToFrontOnFocus);

    float W = io.DisplaySize.x;
    auto centerText = [&](const char* txt, ImVec4 col) {
        ImGui::SetCursorPosX((W - ImGui::CalcTextSize(txt).x) * 0.5f);
        ImGui::TextColored(col, "%s", txt);
    };

    ImGui::Spacing();
    centerText("NHA SACH FETEL", C_VANG);
    centerText("HCMUS - 23DTV_CLC1  |  235 Nguyen Van Cu, Q5, TP.HCM  |  23207022.19.14", C_XAM);
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    /* Thong bao */
    if (!thongBao.empty()) {
        ImGui::SetCursorPosX(20);
        ImGui::TextColored(C_XANH, "%s", thongBao.c_str());
        ImGui::SameLine();
        if (ImGui::SmallButton("x")) thongBao = "";
        ImGui::Spacing();
    }

    /* Thong ke nhanh */
    int tongHang = (int)kho.kho.size();
    int hetHang  = (int)ranges::count_if(kho.kho,[](const auto&h){return h->soLuongTon==0;});
    int sapHet   = (int)ranges::count_if(kho.kho,[](const auto&h){return h->soLuongTon<5&&!h->ngungBan;});

    ImGui::SetCursorPosX(20);
    ImGui::TextColored(C_XAM, "Hang hoa: "); ImGui::SameLine();
    ImGui::TextColored(C_VANG, "%d", tongHang); ImGui::SameLine(0,30);
    ImGui::TextColored(C_XAM, "Hoa don: "); ImGui::SameLine();
    ImGui::TextColored(C_VANG, "%d", (int)lichSu.dsHoaDon.size()); ImGui::SameLine(0,30);
    ImGui::TextColored(C_XAM, "Khach than thiet: "); ImGui::SameLine();
    ImGui::TextColored(C_VANG, "%d", (int)lichSu.dsKH.size()); ImGui::SameLine(0,30);
    if (hetHang > 0) { ImGui::TextColored(C_DO, "[!] Het hang: %d", hetHang); ImGui::SameLine(0,15); }
    if (sapHet  > 0)   ImGui::TextColored(C_CAM, "[!] Sap het: %d", sapHet);

    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    /* Cac nut menu */
    float btnW=210, btnH=80, gap=20;
    float totalW = 5*btnW + 4*gap;
    float startX = (W - totalW) * 0.5f;

    auto bigBtn = [&](const char* label, ImVec4 col, float x, float y) -> bool {
        ImGui::SetCursorPos({x, y});
        ImGui::PushStyleColor(ImGuiCol_Button,        col);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {col.x+0.08f,col.y+0.08f,col.z+0.08f,1.f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  {col.x-0.08f,col.y-0.08f,col.z-0.08f,1.f});
        bool r = ImGui::Button(label, {btnW, btnH});
        ImGui::PopStyleColor(3);
        return r;
    };

    float rowY = 170;
    if (bigBtn("BAN HANG\n(Tao hoa don moi)",    {0.13f,0.62f,0.38f,1.f}, startX,               rowY)) showBanHang    = true;
    if (bigBtn("QUAN LY KHO\n(Hang hoa & ton kho)", {0.13f,0.45f,0.72f,1.f}, startX+(btnW+gap),     rowY)) showKho         = true;
    if (bigBtn("LICH SU\nHOA DON",               {0.50f,0.25f,0.60f,1.f}, startX+2*(btnW+gap),   rowY)) { showLichSu = true; hdDangXem = nullptr; }
    if (bigBtn("KHACH HANG\nTHAN THIET",          {0.70f,0.40f,0.10f,1.f}, startX+3*(btnW+gap),   rowY)) showKhachHang   = true;
    if (bigBtn("BAO CAO\nNGAY",                   {0.50f,0.18f,0.18f,1.f}, startX+4*(btnW+gap),   rowY)) showBaoCao      = true;

    ImGui::End();
}

/* ================================================================
   TRANG: QUAN LY KHO
================================================================ */
void BookstoreApp::renderQuanLyKho() {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos({0,0});
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::Begin("##kho", nullptr,
        ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove);

    if (ImGui::Button("< Menu")) showKho = false;
    ImGui::SameLine(0,20); ImGui::TextColored(C_VANG, "QUAN LY KHO HANG");
    ImGui::SameLine(0,30); ImGui::TextColored(C_XAM, "Tim:"); ImGui::SameLine();
    ImGui::SetNextItemWidth(220); ImGui::InputText("##timkho", buf_tim, sizeof(buf_tim));
    ImGui::SameLine(0,15);
    if (ImGui::Button("Them hang moi")) { resetBuf(); showFormThem = true; }
    ImGui::SameLine();
    if (ImGui::Button("Lam moi")) kho.khoiTao();
    ImGui::Separator(); ImGui::Spacing();

    float tableH = io.DisplaySize.y - 130;
    if (ImGui::BeginTable("tblKho", 7,
        ImGuiTableFlags_Borders|ImGuiTableFlags_RowBg|
        ImGuiTableFlags_ScrollY|ImGuiTableFlags_Resizable, {0,tableH}))
    {
        ImGui::TableSetupScrollFreeze(0,1);
        ImGui::TableSetupColumn("Ma",         ImGuiTableColumnFlags_WidthFixed,   55);
        ImGui::TableSetupColumn("Ten hang",   ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Loai",       ImGuiTableColumnFlags_WidthFixed,  110);
        ImGui::TableSetupColumn("NCC",        ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Gia ban",    ImGuiTableColumnFlags_WidthFixed,   90);
        ImGui::TableSetupColumn("Ton kho",    ImGuiTableColumnFlags_WidthFixed,   70);
        ImGui::TableSetupColumn("Hanh dong",  ImGuiTableColumnFlags_WidthFixed,  130);
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

            ImGui::TableSetColumnIndex(0); ImGui::Text("%d", h->ma);
            ImGui::TableSetColumnIndex(1); ImGui::TextColored(mauLoai(h->loai), "%s", h->ten.c_str());
            ImGui::TableSetColumnIndex(2); ImGui::TextColored(mauLoai(h->loai), "%s", h->tenLoai().c_str());
            ImGui::TableSetColumnIndex(3); ImGui::Text("%s", h->nhaCungCap.c_str());
            ImGui::TableSetColumnIndex(4); ImGui::Text("%.0f", h->giaBan);
            ImGui::TableSetColumnIndex(5);
            if (h->soLuongTon < 5) ImGui::TextColored(C_DO, "%d", h->soLuongTon);
            else ImGui::Text("%d", h->soLuongTon);
            ImGui::TableSetColumnIndex(6);
            ImGui::PushID(h->ma);
            if (ImGui::SmallButton("Nhap them")) {
                hangDangSua = h.get();
                snprintf(buf_sl, sizeof(buf_sl), "0");
                showFormNhap = true;
            }
            ImGui::SameLine();
            if (h->ngungBan) {
                if (ImGui::SmallButton("Mo ban")) { kho.doiTrangThai(h->ma,false); kho.luu(); }
            } else {
                if (ImGui::SmallButton("Ngung"))  { kho.doiTrangThai(h->ma,true);  kho.luu(); }
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }

    /* Thong ke */
    ImGui::Separator();
    int het = (int)ranges::count_if(kho.kho,[](const auto&h){return h->soLuongTon==0;});
    int sap = (int)ranges::count_if(kho.kho,[](const auto&h){return h->soLuongTon<5&&!h->ngungBan;});
    ImGui::Text("Tong: %d mat hang", (int)kho.kho.size());
    ImGui::SameLine(200); ImGui::TextColored(C_DO,  "Het hang: %d", het);
    ImGui::SameLine(320); ImGui::TextColored(C_CAM, "Sap het(<5): %d", sap);
    ImGui::End();

    /* Popup: Them hang moi */
    if (showFormThem) {
        ImGui::SetNextWindowSize({480,420}, ImGuiCond_Always);
        ImGui::Begin("Them Hang Moi", &showFormThem, ImGuiWindowFlags_NoResize);
        const char* dsLoai[] = {"Sach","Vo/But/VPP","Do choi","Do luu niem","Van phong pham"};
        ImGui::Text("Loai hang:"); ImGui::SameLine();
        ImGui::SetNextItemWidth(200); ImGui::Combo("##loai",&loaiChon,dsLoai,5);
        ImGui::Separator();
        float fw = 360;
        ImGui::SetNextItemWidth(fw); ImGui::InputText("Ten hang",      buf_ten,      sizeof(buf_ten));
        ImGui::SetNextItemWidth(fw); ImGui::InputText("Nha cung cap",  buf_ncc,      sizeof(buf_ncc));
        ImGui::SetNextItemWidth(150);ImGui::InputText("Gia nhap (VND)",buf_gia_nhap, sizeof(buf_gia_nhap));
        ImGui::SetNextItemWidth(150);ImGui::InputText("Gia ban (VND)", buf_gia_ban,  sizeof(buf_gia_ban));
        ImGui::SetNextItemWidth(100);ImGui::InputText("So luong",      buf_sl,       sizeof(buf_sl));
        ImGui::Separator();
        if      (loaiChon==0) { ImGui::SetNextItemWidth(fw);  ImGui::InputText("Tac gia", buf_extra1,sizeof(buf_extra1)); ImGui::SetNextItemWidth(fw); ImGui::InputText("The loai",buf_extra2,sizeof(buf_extra2)); }
        else if (loaiChon==1) { ImGui::SetNextItemWidth(80);  ImGui::InputText("Lop tu",  buf_extra1,sizeof(buf_extra1)); ImGui::SameLine(); ImGui::SetNextItemWidth(80); ImGui::InputText("Lop den",buf_extra2,sizeof(buf_extra2)); }
        else if (loaiChon==2) { ImGui::SetNextItemWidth(100); ImGui::InputText("Do tuoi", buf_extra1,sizeof(buf_extra1)); }
        else if (loaiChon==3) { ImGui::SetNextItemWidth(fw);  ImGui::InputText("Chat lieu",buf_extra1,sizeof(buf_extra1)); }
        else { ImGui::TextColored(C_XAM,"(Khong co truong rieng)"); }
        ImGui::Separator();
        ImGui::PushStyleColor(ImGuiCol_Button,{0.13f,0.62f,0.38f,1.f});
        if (ImGui::Button("LUU",{120,32})) {
            if (strlen(buf_ten)==0) { thongBao="[!] Chua nhap ten hang!"; }
            else {
                LoaiHang l=static_cast<LoaiHang>(loaiChon+1);
                kho.them(buf_ten,buf_ncc,
                    (float)atof(buf_gia_nhap),(float)atof(buf_gia_ban),
                    atoi(buf_sl),l,
                    buf_extra1,buf_extra2,
                    atoi(buf_extra1),atoi(buf_extra2),atoi(buf_extra1));
                kho.luu();
                thongBao = format("[OK] Da them '{}' (Ma: {})", buf_ten, kho.kho.back()->ma);
                resetBuf(); showFormThem=false;
            }
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();
        if (ImGui::Button("Huy",{80,32})) { resetBuf(); showFormThem=false; }
        ImGui::End();
    }

    /* Popup: Nhap them */
    if (showFormNhap && hangDangSua) {
        ImGui::SetNextWindowSize({380,180},ImGuiCond_Always);
        ImGui::Begin("Nhap Them Hang",&showFormNhap,ImGuiWindowFlags_NoResize);
        ImGui::TextColored(mauLoai(hangDangSua->loai),"[%s] %s",hangDangSua->tenLoai().c_str(),hangDangSua->ten.c_str());
        ImGui::Text("Ton kho hien tai: %d",hangDangSua->soLuongTon);
        ImGui::SetNextItemWidth(100);ImGui::InputText("So luong nhap them",buf_sl,sizeof(buf_sl));
        int sl=atoi(buf_sl);
        if(sl>0){ImGui::SameLine();ImGui::TextColored(C_XAM,"-> Moi: %d",hangDangSua->soLuongTon+sl);}
        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Button,{0.55f,0.37f,0.06f,1.f});
        if(ImGui::Button("XAC NHAN",{120,32})){
            if(sl>0){kho.nhapThem(hangDangSua->ma,sl);kho.luu();thongBao=format("[OK] Da nhap them {} chiec cho '{}'",sl,hangDangSua->ten);showFormNhap=false;hangDangSua=nullptr;}
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();
        if(ImGui::Button("Huy",{80,32})){showFormNhap=false;hangDangSua=nullptr;}
        ImGui::End();
    }
}

/* ================================================================
   TRANG: BAN HANG
================================================================ */
void BookstoreApp::renderBanHang() {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos({0,0});
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::Begin("##banhang",nullptr,
        ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove);

    if(ImGui::Button("< Menu"))showBanHang=false;
    ImGui::SameLine(0,20);ImGui::TextColored(C_XANH,"BAN HANG - TAO HOA DON  #%04d",lichSu.maHDTiep);
    ImGui::Separator();

    /* Cot trai: danh sach hang */
    ImGui::BeginChild("col_hang",{430,io.DisplaySize.y-80},true);
    ImGui::Text("CHON HANG");ImGui::Separator();
    ImGui::SetNextItemWidth(200);ImGui::InputText("Tim##bh",buf_tim,sizeof(buf_tim));
    if(ImGui::BeginTable("tblBH",4,ImGuiTableFlags_Borders|ImGuiTableFlags_RowBg|ImGuiTableFlags_ScrollY,{0,io.DisplaySize.y-170})){
        ImGui::TableSetupScrollFreeze(0,1);
        ImGui::TableSetupColumn("Ma",ImGuiTableColumnFlags_WidthFixed,45);
        ImGui::TableSetupColumn("Ten",ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Gia",ImGuiTableColumnFlags_WidthFixed,80);
        ImGui::TableSetupColumn("Con",ImGuiTableColumnFlags_WidthFixed,40);
        ImGui::TableHeadersRow();
        string filter=buf_tim;
        for(auto&h:kho.kho){
            if(h->ngungBan||h->soLuongTon<=0)continue;
            if(!filter.empty()&&h->ten.find(filter)==string::npos&&to_string(h->ma).find(filter)==string::npos)continue;
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);ImGui::Text("%d",h->ma);
            ImGui::TableSetColumnIndex(1);
            ImGui::PushID(h->ma);
            if(ImGui::Selectable(h->ten.c_str(),false,ImGuiSelectableFlags_SpanAllColumns)){
                hangDangChon=h.get(); snprintf(buf_sl,sizeof(buf_sl),"1");
            }
            ImGui::PopID();
            ImGui::TableSetColumnIndex(2);ImGui::Text("%.0f",h->giaBan);
            ImGui::TableSetColumnIndex(3);ImGui::Text("%d",h->soLuongTon);
        }
        ImGui::EndTable();
    }
    ImGui::EndChild();

    ImGui::SameLine();

    /* Cot phai: hoa don */
    ImGui::BeginChild("col_hd",{0,io.DisplaySize.y-80},true);
    ImGui::Text("HOA DON #%04d",lichSu.maHDTiep);ImGui::Separator();

    static int nvChon=0;
    const char* dsNV[]={"Nguyen Thi An","Tran Van Binh","Le Thi Cam","Pham Van Dung","Hoang Thi Em"};
    ImGui::Text("Nhan vien:");ImGui::SameLine();
    ImGui::SetNextItemWidth(190);ImGui::Combo("##nv",&nvChon,dsNV,5);

    ImGui::SetNextItemWidth(180);ImGui::InputText("SDT Khach",buf_sdt,sizeof(buf_sdt));
    ImGui::SameLine();
    if(ImGui::Button("Tim KH")){
        if(auto*kh=lichSu.timKH(string(buf_sdt));kh){
            strncpy(buf_ten_kh,kh->hoTen.c_str(),sizeof(buf_ten_kh)-1);
            hdHangKH=static_cast<int>(kh->hang);
            thongBao=format("Tim thay: {} - {} - Giam {:.0f}%",kh->hoTen,kh->tenHang(),kh->tyLeGiam()*100);
        } else { thongBao="Khach moi - Nhap ten ben duoi"; }
    }
    ImGui::SetNextItemWidth(240);ImGui::InputText("Ten khach",buf_ten_kh,sizeof(buf_ten_kh));

    /* Them mat hang */
    ImGui::Separator();
    if(hangDangChon){
        ImGui::TextColored(mauLoai(hangDangChon->loai),"Chon: %s  %.0f VND  (con %d)",
            hangDangChon->ten.c_str(),hangDangChon->giaBan,hangDangChon->soLuongTon);
        ImGui::SetNextItemWidth(70);ImGui::InputText("SL##bh",buf_sl,sizeof(buf_sl));
        ImGui::SameLine();
        if(ImGui::Button("Them vao HD")){
            int sl=atoi(buf_sl);
            if(sl>0&&sl<=hangDangChon->soLuongTon&&hdSoMatHang<MAX_CHITIET){
                float giam=hangDangChon->tinhGiamGia(sl);
                float giaGoc=hangDangChon->giaBan*(float)sl;
                float tt=giaGoc*(1-giam);
                auto&ct=hdChiTiet[hdSoMatHang++];
                ct.maHang=hangDangChon->ma;ct.soLuong=sl;ct.giaBan=hangDangChon->giaBan;
                ct.giamGiaPhanTram=giam;ct.thanhTien=tt;ct.tenHang=hangDangChon->ten;
                hdTongTruocGiam+=giaGoc;hdGiamSoLuong+=giaGoc*giam;
                hangDangChon->soLuongTon-=sl;
                hangDangChon=nullptr;
            }
        }
        ImGui::SameLine();
        if(ImGui::Button("Bo chon"))hangDangChon=nullptr;
    }

    /* Danh sach mat hang da chon */
    ImGui::Separator();
    if(ImGui::BeginTable("tblCT",5,ImGuiTableFlags_Borders|ImGuiTableFlags_RowBg|ImGuiTableFlags_ScrollY,{0,180})){
        ImGui::TableSetupColumn("Ten",     ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("SL",      ImGuiTableColumnFlags_WidthFixed,35);
        ImGui::TableSetupColumn("Gia",     ImGuiTableColumnFlags_WidthFixed,80);
        ImGui::TableSetupColumn("Giam",    ImGuiTableColumnFlags_WidthFixed,50);
        ImGui::TableSetupColumn("T.Tien",  ImGuiTableColumnFlags_WidthFixed,90);
        ImGui::TableHeadersRow();
        for(int i=0;i<hdSoMatHang;i++){
            auto&ct=hdChiTiet[i];
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);ImGui::Text("%s",ct.tenHang.c_str());
            ImGui::TableSetColumnIndex(1);ImGui::Text("%d",ct.soLuong);
            ImGui::TableSetColumnIndex(2);ImGui::Text("%.0f",ct.giaBan);
            ImGui::TableSetColumnIndex(3);
            if(ct.giamGiaPhanTram>0)ImGui::TextColored(C_XANH,"%.0f%%",ct.giamGiaPhanTram*100);
            else ImGui::Text("-");
            ImGui::TableSetColumnIndex(4);ImGui::Text("%.0f",ct.thanhTien);
        }
        ImGui::EndTable();
    }

    /* Tinh tien */
    ImGui::Separator();
    KhachHang* kh = lichSu.timKH(string(buf_sdt));
    float tyLeKH   = kh ? kh->tyLeGiam() : 0.0f;
    float tongSauSL= hdTongTruocGiam - hdGiamSoLuong;
    float giamKH   = tongSauSL * tyLeKH;
    float tongSauGiam = tongSauSL - giamKH;
    float thanhToanLT = lamTron(tongSauGiam);

    ImGui::Text("Tong truoc giam  : %.0f VND",hdTongTruocGiam);
    if(hdGiamSoLuong>0)ImGui::TextColored(C_XANH,"Giam so luong    : %.0f VND",hdGiamSoLuong);
    if(giamKH>0)ImGui::TextColored(C_XANH,"Giam hang KH(%.0f%%): %.0f VND",tyLeKH*100,giamKH);
    ImGui::TextColored(C_VANG,"THANH TOAN (lt)  : %.0f VND",thanhToanLT);

    static char buf_tien_dua[30]={};
    ImGui::SetNextItemWidth(160);ImGui::InputText("Tien khach dua",buf_tien_dua,sizeof(buf_tien_dua));
    float tiendua=(float)atof(buf_tien_dua);
    if(tiendua>=thanhToanLT&&thanhToanLT>0)
        ImGui::TextColored(C_XANH,"Tien thua: %.0f VND",tiendua-thanhToanLT);

    ImGui::Spacing();
    bool coThe=hdSoMatHang>0&&strlen(buf_ten_kh)>0;
    if(!coThe)ImGui::BeginDisabled();
    ImGui::PushStyleColor(ImGuiCol_Button,{0.13f,0.62f,0.38f,1.f});
    if(ImGui::Button("XUAT HOA DON",{160,35})){
        HoaDon hd{};
        hd.maHD=lichSu.maHDTiep++;
        layNgayGio(hd.ngay,hd.gio);
        hd.nhanVien=dsNV[nvChon];
        hd.tenKH=buf_ten_kh; hd.sdtKH=buf_sdt;
        hd.hangKH=kh?static_cast<int>(kh->hang):0;
        hd.soMatHang=hdSoMatHang;
        for(int i=0;i<hdSoMatHang;i++)hd.chiTiet[i]=hdChiTiet[i];
        hd.tongTruocGiam=hdTongTruocGiam; hd.giamSoLuong=hdGiamSoLuong;
        hd.giamKhachHang=giamKH; hd.tongSauGiam=tongSauGiam;
        hd.thanhToanLamTron=thanhToanLT;
        hd.tienKhachDua=tiendua; hd.tienThua=tiendua-thanhToanLT;
        lichSu.capNhatKH(hd.sdtKH,hd.tenKH,thanhToanLT);
        lichSu.themHoaDon(hd);
        kho.luu();
        thongBao=format("[OK] Xuat thanh cong hoa don #{:04d} - {:.0f} VND",hd.maHD,thanhToanLT);
        /* Reset */
        hdSoMatHang=0;hdTongTruocGiam=0;hdGiamSoLuong=0;hdHangKH=0;
        hangDangChon=nullptr;
        memset(buf_sdt,0,sizeof(buf_sdt));
        memset(buf_ten_kh,0,sizeof(buf_ten_kh));
        memset(buf_tien_dua,0,sizeof(buf_tien_dua));
    }
    ImGui::PopStyleColor();
    if(!coThe)ImGui::EndDisabled();
    ImGui::SameLine();
    if(ImGui::Button("Huy HD",{80,35})){
        for(int i=0;i<hdSoMatHang;i++){
            for(auto&h:kho.kho)if(h->ma==hdChiTiet[i].maHang){h->soLuongTon+=hdChiTiet[i].soLuong;break;}
        }
        hdSoMatHang=0;hdTongTruocGiam=0;hdGiamSoLuong=0;hangDangChon=nullptr;
    }
    ImGui::EndChild();
    ImGui::End();
}

/* ================================================================
   TRANG: LICH SU HOA DON
================================================================ */
void BookstoreApp::renderLichSuHoaDon() {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos({0,0});
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::Begin("##lichsu",nullptr,
        ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove);

    if(ImGui::Button("< Menu"))showLichSu=false;
    ImGui::SameLine(0,20);ImGui::TextColored(C_TIM,"LICH SU HOA DON (%d don)",(int)lichSu.dsHoaDon.size());
    ImGui::SameLine(0,20);ImGui::TextColored(C_XAM,"Tim:");ImGui::SameLine();
    ImGui::SetNextItemWidth(220);ImGui::InputText("##timhd",buf_tim,sizeof(buf_tim));
    ImGui::Separator();

    float topH = hdDangXem ? io.DisplaySize.y*0.45f : io.DisplaySize.y-80;
    if(ImGui::BeginTable("tblLS",7,
        ImGuiTableFlags_Borders|ImGuiTableFlags_RowBg|
        ImGuiTableFlags_ScrollY|ImGuiTableFlags_Resizable,{0,topH}))
    {
        ImGui::TableSetupScrollFreeze(0,1);
        ImGui::TableSetupColumn("So HD",    ImGuiTableColumnFlags_WidthFixed, 65);
        ImGui::TableSetupColumn("Ngay",     ImGuiTableColumnFlags_WidthFixed, 95);
        ImGui::TableSetupColumn("Gio",      ImGuiTableColumnFlags_WidthFixed, 75);
        ImGui::TableSetupColumn("Nhan vien",ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Khach",    ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Hang KH",  ImGuiTableColumnFlags_WidthFixed, 85);
        ImGui::TableSetupColumn("Thanh toan",ImGuiTableColumnFlags_WidthFixed,105);
        ImGui::TableHeadersRow();

        string filter=buf_tim;
        for(auto&hd:lichSu.dsHoaDon){
            if(!filter.empty()){
                bool ok=hd.tenKH.find(filter)!=string::npos||
                        hd.nhanVien.find(filter)!=string::npos||
                        to_string(hd.maHD).find(filter)!=string::npos||
                        string(hd.ngay).find(filter)!=string::npos;
                if(!ok)continue;
            }
            ImGui::TableNextRow();
            if(hd.daHuy)ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,
                ImGui::ColorConvertFloat4ToU32({0.4f,0.1f,0.1f,0.4f}));
            ImGui::TableSetColumnIndex(0);
            bool sel=false;
            if(ImGui::Selectable(format("#{:04d}",hd.maHD).c_str(),&sel,
                ImGuiSelectableFlags_SpanAllColumns))hdDangXem=&hd;
            ImGui::TableSetColumnIndex(1);ImGui::Text("%s",hd.ngay);
            ImGui::TableSetColumnIndex(2);ImGui::Text("%s",hd.gio);
            ImGui::TableSetColumnIndex(3);ImGui::Text("%s",hd.nhanVien.c_str());
            ImGui::TableSetColumnIndex(4);ImGui::Text("%s",hd.tenKH.c_str());
            ImGui::TableSetColumnIndex(5);ImGui::Text("%s",hd.tenHangKH().data());
            ImGui::TableSetColumnIndex(6);ImGui::Text("%.0f",hd.thanhToanLamTron);
        }
        ImGui::EndTable();
    }

    /* Chi tiet hoa don duoc chon */
    if(hdDangXem){
        ImGui::Separator();
        ImGui::TextColored(C_VANG,"CHI TIET HOA DON #%04d",hdDangXem->maHD);
        ImGui::Text("Ngay: %s  Gio: %s  NV: %s",hdDangXem->ngay,hdDangXem->gio,hdDangXem->nhanVien.c_str());
        ImGui::Text("Khach: %s  SDT: %s  Hang: %s",hdDangXem->tenKH.c_str(),hdDangXem->sdtKH.c_str(),hdDangXem->tenHangKH().data());
        if(ImGui::BeginTable("tblCTXem",5,ImGuiTableFlags_Borders|ImGuiTableFlags_RowBg)){
            ImGui::TableSetupColumn("Ten hang",ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("SL",   ImGuiTableColumnFlags_WidthFixed,40);
            ImGui::TableSetupColumn("Don gia",ImGuiTableColumnFlags_WidthFixed,90);
            ImGui::TableSetupColumn("Giam%",ImGuiTableColumnFlags_WidthFixed,60);
            ImGui::TableSetupColumn("T.Tien",ImGuiTableColumnFlags_WidthFixed,100);
            ImGui::TableHeadersRow();
            for(int i=0;i<hdDangXem->soMatHang;i++){
                auto&ct=hdDangXem->chiTiet[i];
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);ImGui::Text("%s",ct.tenHang.c_str());
                ImGui::TableSetColumnIndex(1);ImGui::Text("%d",ct.soLuong);
                ImGui::TableSetColumnIndex(2);ImGui::Text("%.0f",ct.giaBan);
                ImGui::TableSetColumnIndex(3);
                if(ct.giamGiaPhanTram>0)ImGui::TextColored(C_XANH,"%.0f%%",ct.giamGiaPhanTram*100);
                else ImGui::Text("-");
                ImGui::TableSetColumnIndex(4);ImGui::Text("%.0f",ct.thanhTien);
            }
            ImGui::EndTable();
        }
        ImGui::Text("Tong: %.0f  Giam SL: %.0f  Giam KH: %.0f",
            hdDangXem->tongTruocGiam,hdDangXem->giamSoLuong,hdDangXem->giamKhachHang);
        ImGui::TextColored(C_VANG,"THANH TOAN: %.0f VND  |  Tien thua: %.0f VND",
            hdDangXem->thanhToanLamTron,hdDangXem->tienThua);
        if(ImGui::Button("Dong chi tiet"))hdDangXem=nullptr;
    }
    ImGui::End();
}

/* ================================================================
   TRANG: KHACH HANG
================================================================ */
void BookstoreApp::renderKhachHang() {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos({0,0});
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::Begin("##kh",nullptr,
        ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove);

    if(ImGui::Button("< Menu"))showKhachHang=false;
    ImGui::SameLine(0,20);
    ImGui::TextColored(C_CAM,"KHACH HANG THAN THIET (%d khach)",(int)lichSu.dsKH.size());
    ImGui::Separator();

    if(ImGui::BeginTable("tblKH",6,
        ImGuiTableFlags_Borders|ImGuiTableFlags_RowBg|ImGuiTableFlags_ScrollY,
        {0,io.DisplaySize.y-120}))
    {
        ImGui::TableSetupScrollFreeze(0,1);
        ImGui::TableSetupColumn("SDT",       ImGuiTableColumnFlags_WidthFixed,  130);
        ImGui::TableSetupColumn("Ho ten",    ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Hang",      ImGuiTableColumnFlags_WidthFixed,   90);
        ImGui::TableSetupColumn("So don",    ImGuiTableColumnFlags_WidthFixed,   70);
        ImGui::TableSetupColumn("Tong chi",  ImGuiTableColumnFlags_WidthFixed,  120);
        ImGui::TableSetupColumn("Diem",      ImGuiTableColumnFlags_WidthFixed,   65);
        ImGui::TableHeadersRow();

        for(auto&k:lichSu.dsKH){
            ImVec4 mauHang=C_XAM;
            if(k.hang==HangKH::VVIP)mauHang=C_DO;
            else if(k.hang==HangKH::VIP)mauHang=C_TIM;
            else if(k.hang==HangKH::Than)mauHang=C_XANH;
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);ImGui::Text("%s",k.sdt.c_str());
            ImGui::TableSetColumnIndex(1);ImGui::TextColored(mauHang,"%s",k.hoTen.c_str());
            ImGui::TableSetColumnIndex(2);ImGui::TextColored(mauHang,"%s",k.tenHang().data());
            ImGui::TableSetColumnIndex(3);ImGui::Text("%d",k.tongDonHang);
            ImGui::TableSetColumnIndex(4);ImGui::Text("%.0f",k.tongChiTieu);
            ImGui::TableSetColumnIndex(5);ImGui::Text("%d",k.diemTichLuy);
        }
        ImGui::EndTable();
    }
    ImGui::Separator();
    ImGui::TextColored(C_DO,"[Do] VVIP (>=1500 diem, giam 15%%)"); ImGui::SameLine(260);
    ImGui::TextColored(C_TIM,"[Tim] VIP (>=500, giam 10%%)");      ImGui::SameLine(470);
    ImGui::TextColored(C_XANH,"[Xanh] Than thiet (>=200, giam 5%%)");
    ImGui::End();
}

/* ================================================================
   TRANG: BAO CAO NGAY
================================================================ */
void BookstoreApp::renderBaoCaoNgay() {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos({0,0});
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::Begin("##baocao",nullptr,
        ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove);

    if(ImGui::Button("< Menu"))showBaoCao=false;
    ImGui::SameLine(0,20);ImGui::TextColored(C_DO,"BAO CAO HOAT DONG NGAY");

    char ngayHom[12],gioHom[9]; layNgayGio(ngayHom,gioHom);
    ImGui::SameLine(0,30);ImGui::TextColored(C_XAM,"Ngay: %s  Gio: %s",ngayHom,gioHom);
    ImGui::Separator();

    int soHDN=0,soHuy=0; float dtThuc=0;
    float nvDT[5]={}; int nvDon[5]={};
    const char* dsNV[]={"Nguyen Thi An","Tran Van Binh","Le Thi Cam","Pham Van Dung","Hoang Thi Em"};
    for(auto&hd:lichSu.dsHoaDon){
        if(strcmp(hd.ngay,ngayHom)==0){
            soHDN++;
            if(!hd.daHuy)dtThuc+=hd.thanhToanLamTron; else soHuy++;
            for(int j=0;j<5;j++)if(hd.nhanVien==dsNV[j]){nvDT[j]+=hd.thanhToanLamTron;nvDon[j]++;}
        }
    }
    int tongTon=0;float gtTon=0;
    int het=(int)ranges::count_if(kho.kho,[](const auto&h){return h->soLuongTon==0;});
    int sap=(int)ranges::count_if(kho.kho,[](const auto&h){return h->soLuongTon<5&&!h->ngungBan;});
    for(const auto&h:kho.kho){tongTon+=h->soLuongTon;gtTon+=h->soLuongTon*h->giaNhap;}

    if(ImGui::CollapsingHeader("DOANH THU",ImGuiTreeNodeFlags_DefaultOpen)){
        ImGui::Columns(3,"dt",false);
        ImGui::Text("Tong hoa don: %d",soHDN);ImGui::NextColumn();
        ImGui::Text("Thanh cong: %d",soHDN-soHuy);ImGui::NextColumn();
        ImGui::TextColored(C_DO,"Huy: %d",soHuy);ImGui::NextColumn();
        ImGui::Columns(1);
        ImGui::TextColored(C_VANG,"Doanh thu thuc: %.0f VND",dtThuc);
    }
    if(ImGui::CollapsingHeader("TON KHO",ImGuiTreeNodeFlags_DefaultOpen)){
        ImGui::Text("Mat hang: %d  |  So luong: %d  |  Gia tri: %.0f VND",(int)kho.kho.size(),tongTon,gtTon);
        ImGui::TextColored(het>0?C_DO:C_XANH,"Het hang: %d",het);ImGui::SameLine(200);
        ImGui::TextColored(sap>0?C_CAM:C_XANH,"Sap het(<5): %d",sap);
        if(het>0||sap>0)ImGui::TextColored(C_DO,"[!] Can nhap them %d mat hang!",het+sap);
        else ImGui::TextColored(C_XANH,"[OK] Ton kho on dinh.");
    }
    if(ImGui::CollapsingHeader("HIEU SUAT NHAN VIEN",ImGuiTreeNodeFlags_DefaultOpen)){
        if(ImGui::BeginTable("tblNV",3,ImGuiTableFlags_Borders|ImGuiTableFlags_RowBg)){
            ImGui::TableSetupColumn("Nhan vien",ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("So don",   ImGuiTableColumnFlags_WidthFixed,80);
            ImGui::TableSetupColumn("Doanh thu",ImGuiTableColumnFlags_WidthFixed,130);
            ImGui::TableHeadersRow();
            for(int j=0;j<5;j++){
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                if(nvDon[j]>0)ImGui::TextColored(C_XANH,"%s",dsNV[j]);
                else ImGui::TextColored(C_XAM,"%s",dsNV[j]);
                ImGui::TableSetColumnIndex(1);ImGui::Text("%d",nvDon[j]);
                ImGui::TableSetColumnIndex(2);ImGui::Text("%.0f VND",nvDT[j]);
            }
            ImGui::EndTable();
        }
    }
    ImGui::End();
}

/* ================================================================
   VONG LAP CHINH
================================================================ */
void BookstoreApp::resetBuf() {
    memset(buf_ten,0,sizeof(buf_ten)); memset(buf_ncc,0,sizeof(buf_ncc));
    memset(buf_gia_nhap,0,sizeof(buf_gia_nhap)); memset(buf_gia_ban,0,sizeof(buf_gia_ban));
    memset(buf_sl,0,sizeof(buf_sl)); memset(buf_extra1,0,sizeof(buf_extra1));
    memset(buf_extra2,0,sizeof(buf_extra2)); loaiChon=0;
}

void BookstoreApp::run() {
    if(!glfwInit()){fprintf(stderr,"GLFW init failed\n");return;}
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window=glfwCreateWindow(1280,720,
        "Nha Sach Fetel - HCMUS - 23DTV_CLC1",nullptr,nullptr);
    if(!window){glfwTerminate();return;}
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().ConfigFlags|=ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGuiStyle&st=ImGui::GetStyle();
    st.WindowRounding=6.f; st.FrameRounding=4.f;
    st.ItemSpacing={8,7};  st.WindowPadding={14,12};
    st.Colors[ImGuiCol_WindowBg]={0.08f,0.08f,0.10f,1.f};
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
        if(showBaoCao)    renderBaoCaoNgay();

        ImGui::Render();
        int dw,dh; glfwGetFramebufferSize(window,&dw,&dh);
        glViewport(0,0,dw,dh);
        glClearColor(0.08f,0.08f,0.10f,1.f);
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