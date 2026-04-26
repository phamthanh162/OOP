/*
  Khohang.cpp - Logic quan ly kho, KHONG co UI console
  Giao dien duoc xu ly boi BookstoreApp.cpp (ImGui)
*/
#include "Khohang.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ranges>
#include <format>
#include <cstring>
#include <cstdio>
using namespace std;

/* ================================================================ TIEN ICH */
static string trimStr(string_view s) {
    auto a = s.find_first_not_of(" \t\r\n");
    if (a == string_view::npos) return "";
    return string(s.substr(a, s.find_last_not_of(" \t\r\n") - a + 1));
}
static string layGiaTri(string_view d) {
    auto p = d.find(':');
    return p == string_view::npos ? "" : trimStr(d.substr(p + 1));
}

/* ================================================================ FACTORY */
unique_ptr<HangHoa> taoHangHoa(LoaiHang l) {
    switch (l) {
        case LoaiHang::Sach:        return make_unique<Sach>();
        case LoaiHang::VoPhanViet:  return make_unique<VoPhanViet>();
        case LoaiHang::DoChoi:      return make_unique<DoChoi>();
        case LoaiHang::LuuNiem:     return make_unique<DoLuuNiem>();
        default:                    return make_unique<VanPhongPham>();
    }
}

/* ================================================================ FILE I/O */
void QuanLyKho::ghiCSV() {
    FILE* f = fopen("nhasach.csv", "wb");
    if (!f) return;
    fprintf(f, "\xEF\xBB\xBF"
        "Ma;Ten;Loai;NhaCungCap;GiaNhap;GiaBan;TonKho;NgungBan;"
        "TacGia;TheLoai;LopTu;LopDen;DoTuoi;ChatLieu\r\n");
    for (const auto& h : kho)
        fprintf(f, "%d;%s;%d;%s;%.0f;%.0f;%d;%d;%s;%s;%d;%d;%d;%s\r\n",
            h->ma, h->ten.c_str(), h->maLoai(), h->nhaCungCap.c_str(),
            h->giaNhap, h->giaBan, h->soLuongTon, h->ngungBan ? 1 : 0,
            h->loai == LoaiHang::Sach       ? h->tacGia().c_str()   : "",
            h->loai == LoaiHang::Sach       ? h->theLoai().c_str()  : "",
            h->loai == LoaiHang::VoPhanViet ? h->lopTu()  : 0,
            h->loai == LoaiHang::VoPhanViet ? h->lopDen() : 0,
            h->loai == LoaiHang::DoChoi     ? h->doTuoi() : 0,
            h->loai == LoaiHang::LuuNiem    ? h->chatLieu().c_str() : "");
    fclose(f);
}

void QuanLyKho::ghiTXT() {
    ofstream f("nhasach.txt");
    if (!f) return;
    f << format("================================================================\n"
                "  NHA SACH FETEL | HANG HOA | Tong: {} | Ma tiep: {}\n"
                "================================================================\n\n",
                kho.size(), maHangTiep);
    for (const auto& h : kho) {
        f << format("[HANG #{:04d}]\n"
                    "  Ten          : {}\n"
                    "  Loai         : {}\n"
                    "  Nha cung cap : {}\n"
                    "  Gia nhap     : {:.0f} VND\n"
                    "  Gia ban      : {:.0f} VND\n"
                    "  Ton kho      : {}\n"
                    "  Ngung ban    : {}\n",
                    h->ma, h->ten, h->tenLoai(), h->nhaCungCap,
                    h->giaNhap, h->giaBan, h->soLuongTon,
                    h->ngungBan ? "Co" : "Khong");
        if (h->loai == LoaiHang::Sach)
            f << "  Tac gia      : " << h->tacGia() << "\n"
              << "  The loai     : " << h->theLoai() << "\n";
        if (h->loai == LoaiHang::VoPhanViet)
            f << format("  Phu hop lop  : {} den {}\n", h->lopTu(), h->lopDen());
        if (h->loai == LoaiHang::DoChoi)
            f << format("  Do tuoi      : {}+\n", h->doTuoi());
        if (h->loai == LoaiHang::LuuNiem)
            f << "  Chat lieu    : " << h->chatLieu() << "\n";
        f << "\n";
    }
    f << "================================================================\n"
         "  GHI CHU: Co the chinh sua file nay.\n"
         "  KHONG CHINH SUA: Ma hang.\n"
         "================================================================\n";
}

void QuanLyKho::docTXT() {
    ifstream f("nhasach.txt");
    if (!f) return;
    kho.clear(); maHangTiep = 1001;
    string dong;
    unique_ptr<HangHoa> h = nullptr;
    while (getline(f, dong)) {
        dong = trimStr(dong);
        if (dong.find("Ma tiep") != string::npos) {
            if (auto p = dong.rfind(':'); p != string::npos)
                maHangTiep = stoi(trimStr(dong.substr(p + 1)));
            continue;
        }
        if (dong.size() > 6 && dong.substr(0, 7) == "[HANG #") {
            if (h) kho.push_back(std::move(h));
            h = make_unique<Sach>();
            h->ma = stoi(dong.substr(7));
            continue;
        }
        if (!h) continue;
        auto val = layGiaTri(dong);
        auto chk = [&](string_view pre) {
            return dong.size() >= pre.size() && dong.substr(0, pre.size()) == pre;
        };
        if      (chk("Ten          :")) h->ten         = val;
        else if (chk("Nha cung cap :")) h->nhaCungCap  = val;
        else if (chk("Gia nhap     :")) h->giaNhap     = stof(val);
        else if (chk("Gia ban      :")) h->giaBan      = stof(val);
        else if (chk("Ton kho      :")) h->soLuongTon  = stoi(val);
        else if (chk("Ngung ban    :")) h->ngungBan    = (val == "Co");
        else if (chk("Loai         :")) {
            LoaiHang ml = LoaiHang::VanPhongPham;
            if      (val.find("Sach")  != string::npos) ml = LoaiHang::Sach;
            else if (val.find("Vo")    != string::npos) ml = LoaiHang::VoPhanViet;
            else if (val.find("choi")  != string::npos) ml = LoaiHang::DoChoi;
            else if (val.find("niem")  != string::npos) ml = LoaiHang::LuuNiem;
            if (ml != h->loai) {
                auto nh = taoHangHoa(ml);
                nh->ma = h->ma; nh->ten = h->ten;
                nh->nhaCungCap = h->nhaCungCap;
                nh->giaNhap = h->giaNhap; nh->giaBan = h->giaBan;
                nh->soLuongTon = h->soLuongTon; nh->ngungBan = h->ngungBan;
                h = std::move(nh);
            }
        }
        else if (chk("Tac gia      :")) {
            if (h->loai == LoaiHang::Sach)
                static_cast<Sach*>(h.get())->_tacGia = val;
        }
        else if (chk("The loai     :")) {
            if (h->loai == LoaiHang::Sach)
                static_cast<Sach*>(h.get())->_theLoai = val;
        }
        else if (chk("Phu hop lop  :")) {
            if (h->loai == LoaiHang::VoPhanViet) {
                int a = 0, b = 0;
                sscanf(val.c_str(), "%d den %d", &a, &b);
                auto* vp = static_cast<VoPhanViet*>(h.get());
                vp->_lopTu = a; vp->_lopDen = b;
            }
        }
        else if (chk("Do tuoi      :")) {
            if (h->loai == LoaiHang::DoChoi)
                static_cast<DoChoi*>(h.get())->_doTuoi = stoi(val);
        }
        else if (chk("Chat lieu    :")) {
            if (h->loai == LoaiHang::LuuNiem)
                static_cast<DoLuuNiem*>(h.get())->_chatLieu = val;
        }
    }
    if (h) kho.push_back(std::move(h));
}

/* ================================================================ PUBLIC */
void QuanLyKho::luu() { ghiTXT(); ghiCSV(); }

void QuanLyKho::khoiTao() {
    docTXT();
    if (kho.empty()) khoiTaoDuLieuMau();
}

void QuanLyKho::them(string_view ten, string_view ncc,
                     float gN, float gB, int sl, LoaiHang l,
                     string_view extra1, string_view extra2,
                     int n1, int n2, int n3) {
    auto h = taoHangHoa(l);
    h->ma = maHangTiep++;
    h->ten = ten; h->nhaCungCap = ncc;
    h->giaNhap = gN; h->giaBan = gB; h->soLuongTon = sl;
    if (l == LoaiHang::Sach) {
        static_cast<Sach*>(h.get())->_tacGia  = extra1;
        static_cast<Sach*>(h.get())->_theLoai = extra2;
    }
    if (l == LoaiHang::VoPhanViet) {
        static_cast<VoPhanViet*>(h.get())->_lopTu  = n1;
        static_cast<VoPhanViet*>(h.get())->_lopDen = n2;
    }
    if (l == LoaiHang::DoChoi)
        static_cast<DoChoi*>(h.get())->_doTuoi = n3;
    if (l == LoaiHang::LuuNiem)
        static_cast<DoLuuNiem*>(h.get())->_chatLieu = extra1;
    kho.push_back(std::move(h));
}

bool QuanLyKho::nhapThem(int ma, int sl) {
    auto it = ranges::find_if(kho, [ma](const auto& h) { return h->ma == ma; });
    if (it == kho.end()) return false;
    (*it)->soLuongTon += sl;
    return true;
}

bool QuanLyKho::doiTrangThai(int ma, bool ngung) {
    auto it = ranges::find_if(kho, [ma](const auto& h) { return h->ma == ma; });
    if (it == kho.end()) return false;
    (*it)->ngungBan = ngung;
    return true;
}

void QuanLyKho::khoiTaoDuLieuMau() {
    using L = LoaiHang;
    them("Truyen Kieu",       "NXB Giao Duc",60000, 85000,20,L::Sach,      "Nguyen Du",      "Tho",       0, 0,0);
    them("So Do",             "NXB Van Hoc", 45000, 65000,15,L::Sach,      "Vu Trong Phung", "Truyen",    0, 0,0);
    them("Lap Trinh C Co Ban","NXB DHQG",    80000,120000,15,L::Sach,      "Pham Van At",    "Giao trinh",0, 0,0);
    them("Co So Du Lieu",     "NXB DHQG",    90000,135000, 5,L::Sach,      "Nguyen Ba Tuyen","Giao trinh",0, 0,0);
    them("Doraemon Tap 1",    "NXB Kim Dong",22000, 35000,50,L::Sach,      "Fujiko F.",      "Truyen tranh",0,0,0);
    them("Hoang Tu Be",       "NXB Van Hoc", 28000, 42000,35,L::Sach,      "Antoine de S.",  "Truyen",    0, 0,0);
    them("Tri Tue Nhan Tao",  "NXB DHQG",  100000,160000,10,L::Sach,      "Dinh Manh Tuong","Giao trinh",0, 0,0);
    them("Vo Ke Ngang 200tr", "Thien Long",  8000, 15000,100,L::VoPhanViet,"","",1, 5,0);
    them("But Bi TL 0.5mm",   "Thien Long",  3000,  6000,200,L::VoPhanViet,"","",1,12,0);
    them("But Chi 2B",        "Staedtler",   2000,  4500,140,L::VoPhanViet,"","",1, 9,0);
    them("Gom Trang 4B",      "Staedtler",   1500,  3500,180,L::VoPhanViet,"","",1, 9,0);
    them("Cap Sach HCMUS",    "Fetel Store", 80000,150000,20,L::VoPhanViet,"","",1,12,0);
    them("Xe o to dieu khien","Mattel",      80000,150000,15,L::DoChoi,    "","",0, 0,5);
    them("Bo xep hinh Lego",  "Lego",       120000,220000,10,L::DoChoi,    "","",0, 0,6);
    them("Bup be Baby Barbie","Barbie",      60000,110000,20,L::DoChoi,    "","",0, 0,3);
    them("Moc khoa HCMUS",    "Fetel Store", 15000, 35000,40,L::LuuNiem,   "Kim loai","",0,0,0);
    them("Tranh treo HCMUS",  "Fetel Store", 50000, 95000,10,L::LuuNiem,   "Go ep","",0,0,0);
    them("Coc su HCMUS",      "Fetel Store", 30000, 65000, 5,L::LuuNiem,   "Su cao cap","",0,0,0);
    them("Thuoc ke 30cm",     "Thien Long",   2000,  5000,60,L::VanPhongPham);
    them("Kim bach tac",      "Thien Long",   5000, 12000,80,L::VanPhongPham);
    luu();
}