/*
  LichSuHoaDon.cpp
  Logic quan ly lich su hoa don + khach hang
  KHONG co UI console - giao dien xu ly boi BookstoreApp.cpp (ImGui)
*/
#include "LichSuHoaDon.h"
#include <fstream>
#include <format>
#include <algorithm>
#include <cstring>
#include <cstdio>
using namespace std;

/* ================================================================
   TIEN ICH NOI BO
================================================================ */
static string trimLS(string_view s) {
    auto a = s.find_first_not_of(" \t\r\n");
    if (a == string_view::npos) return "";
    return string(s.substr(a, s.find_last_not_of(" \t\r\n") - a + 1));
}
static string layGiaTriLS(string_view d) {
    auto p = d.find(':');
    return p == string_view::npos ? "" : trimLS(d.substr(p + 1));
}

/* ================================================================
   PHUONG THUC: KHACHHANG
================================================================ */
void KhachHang::capNhatHang() noexcept {
    hang = diemTichLuy >= 1500 ? HangKH::VVIP :
           diemTichLuy >=  500 ? HangKH::VIP  :
           diemTichLuy >=  200 ? HangKH::Than  : HangKH::Thuong;
}

void KhachHang::themDon(float chiTieu) {
    tongDonHang++;
    tongChiTieu += chiTieu;
    diemTichLuy += static_cast<int>(chiTieu / 10000);
    capNhatHang();
}

string_view KhachHang::tenHang() const noexcept {
    switch (hang) {
        case HangKH::VVIP: return "VVIP";
        case HangKH::VIP:  return "VIP";
        case HangKH::Than: return "Than thiet";
        default:           return "Thuong";
    }
}

float KhachHang::tyLeGiam() const noexcept {
    switch (hang) {
        case HangKH::VVIP: return 0.15f;
        case HangKH::VIP:  return 0.10f;
        case HangKH::Than: return 0.05f;
        default:           return 0.0f;
    }
}

/* ================================================================
   PHUONG THUC: HOADON
================================================================ */
unsigned int HoaDon::tinhChecksum() const noexcept {
    unsigned int s = static_cast<unsigned>(maHD)
                   + static_cast<unsigned>(tongTruocGiam * 100)
                   + static_cast<unsigned>(thanhToanLamTron * 100);
    for (auto c : nhanVien) s += static_cast<unsigned char>(c);
    for (int i = 0; i < soMatHang; i++) {
        s += static_cast<unsigned>(chiTiet[i].thanhTien * 100);
        s += static_cast<unsigned>(chiTiet[i].soLuong);
    }
    return s ^ 0xFE7E1123u;
}

string_view HoaDon::tenHangKH() const noexcept {
    if (hangKH == static_cast<int>(HangKH::VVIP)) return "VVIP";
    if (hangKH == static_cast<int>(HangKH::VIP))  return "VIP";
    if (hangKH == static_cast<int>(HangKH::Than)) return "Than thiet";
    return "Thuong";
}

float HoaDon::tyLeGiamKH() const noexcept {
    if (hangKH == static_cast<int>(HangKH::VVIP)) return 0.15f;
    if (hangKH == static_cast<int>(HangKH::VIP))  return 0.10f;
    if (hangKH == static_cast<int>(HangKH::Than)) return 0.05f;
    return 0.0f;
}

/* ================================================================
   GHI FILE HOA DON
================================================================ */
void LichSuHoaDon::ghiHDBIN() {
    FILE* f = fopen("hoadon.dat", "wb");
    if (!f) return;
    int n = static_cast<int>(dsHoaDon.size());
    fwrite(&n,        sizeof(int), 1, f);
    fwrite(&maHDTiep, sizeof(int), 1, f);
    for (const auto& hd : dsHoaDon) {
        HoaDonBin b{};
        b.maHD      = hd.maHD;
        b.hangKH    = hd.hangKH;
        b.soMatHang = hd.soMatHang;
        b.daHuy     = hd.daHuy ? 1 : 0;
        strncpy(b.ngay,     hd.ngay,            11);
        strncpy(b.gio,      hd.gio,              8);
        strncpy(b.nhanVien, hd.nhanVien.c_str(), 59);
        strncpy(b.tenKH,    hd.tenKH.c_str(),    79);
        strncpy(b.sdtKH,    hd.sdtKH.c_str(),    14);
        b.tongTruocGiam    = hd.tongTruocGiam;
        b.giamSoLuong      = hd.giamSoLuong;
        b.giamKhachHang    = hd.giamKhachHang;
        b.tongSauGiam      = hd.tongSauGiam;
        b.thanhToanLamTron = hd.thanhToanLamTron;
        b.tienKhachDua     = hd.tienKhachDua;
        b.tienThua         = hd.tienThua;
        b.checksum         = hd.tinhChecksum();
        for (int j = 0; j < hd.soMatHang && j < MAX_CHITIET; j++) {
            b.ct[j].maHang          = hd.chiTiet[j].maHang;
            b.ct[j].soLuong         = hd.chiTiet[j].soLuong;
            b.ct[j].giaBan          = hd.chiTiet[j].giaBan;
            b.ct[j].giamGiaPhanTram = hd.chiTiet[j].giamGiaPhanTram;
            b.ct[j].thanhTien       = hd.chiTiet[j].thanhTien;
            strncpy(b.ct[j].tenHang, hd.chiTiet[j].tenHang.c_str(), 99);
        }
        fwrite(&b, sizeof(HoaDonBin), 1, f);
    }
    fclose(f);
}

void LichSuHoaDon::docHDBIN() {
    FILE* f = fopen("hoadon.dat", "rb");
    if (!f) return;
    dsHoaDon.clear();
    int tong = 0;
    fread(&tong,      sizeof(int), 1, f);
    fread(&maHDTiep,  sizeof(int), 1, f);
    for (int i = 0; i < tong; i++) {
        HoaDonBin b{};
        fread(&b, sizeof(HoaDonBin), 1, f);
        HoaDon hd{};
        hd.maHD      = b.maHD;
        hd.hangKH    = b.hangKH;
        hd.soMatHang = b.soMatHang;
        hd.daHuy     = (b.daHuy != 0);
        strncpy(hd.ngay, b.ngay, 11);
        strncpy(hd.gio,  b.gio,  8);
        hd.nhanVien         = b.nhanVien;
        hd.tenKH            = b.tenKH;
        hd.sdtKH            = b.sdtKH;
        hd.tongTruocGiam    = b.tongTruocGiam;
        hd.giamSoLuong      = b.giamSoLuong;
        hd.giamKhachHang    = b.giamKhachHang;
        hd.tongSauGiam      = b.tongSauGiam;
        hd.thanhToanLamTron = b.thanhToanLamTron;
        hd.tienKhachDua     = b.tienKhachDua;
        hd.tienThua         = b.tienThua;
        hd.checksum         = b.checksum;
        for (int j = 0; j < b.soMatHang && j < MAX_CHITIET; j++) {
            hd.chiTiet[j].maHang          = b.ct[j].maHang;
            hd.chiTiet[j].soLuong         = b.ct[j].soLuong;
            hd.chiTiet[j].giaBan          = b.ct[j].giaBan;
            hd.chiTiet[j].giamGiaPhanTram = b.ct[j].giamGiaPhanTram;
            hd.chiTiet[j].thanhTien       = b.ct[j].thanhTien;
            hd.chiTiet[j].tenHang         = b.ct[j].tenHang;
        }
        if (hd.tinhChecksum() == hd.checksum)
            dsHoaDon.push_back(hd);
    }
    fclose(f);
}

void LichSuHoaDon::ghiHDTXT() {
    ofstream f("hoadon.txt");
    if (!f) return;
    f << format("================================================================\n"
                "  NHA SACH FETEL | LICH SU HOA DON | Tong: {} don\n"
                "================================================================\n\n",
                dsHoaDon.size());
    for (const auto& hd : dsHoaDon) {
        char buf[200];
        f << "+----------------------------------------------------------+\n";
        sprintf_s(buf, sizeof(buf),
            "| HD #%04d  |  %s  %s  |  NV: %-20s |",
            hd.maHD, hd.ngay, hd.gio, hd.nhanVien.c_str());
        f << buf << "\n";
        sprintf_s(buf, sizeof(buf),
            "| Khach: %-20s  SDT: %-13s  Hang: %-8s|",
            hd.tenKH.c_str(), hd.sdtKH.c_str(), hd.tenHangKH().data());
        f << buf << "\n";
        f << "+--------+-------------------------------+-----+-----------+\n"
             "| Ma     | Ten hang                      |  SL | Thanh tien|\n"
             "+--------+-------------------------------+-----+-----------+\n";
        for (int j = 0; j < hd.soMatHang; j++) {
            sprintf_s(buf, sizeof(buf),
                "| %-6d | %-29s | %3d | %9.0f |",
                hd.chiTiet[j].maHang,
                hd.chiTiet[j].tenHang.c_str(),
                hd.chiTiet[j].soLuong,
                hd.chiTiet[j].thanhTien);
            f << buf << "\n";
        }
        f << format("+--------+-------------------------------+-----+-----------+\n"
                    "  Tong: {:.0f}  Giam SL: {:.0f}  Giam KH: {:.0f}"
                    "  THANH TOAN: {:.0f}  Thua: {:.0f}\n\n",
                    hd.tongTruocGiam, hd.giamSoLuong, hd.giamKhachHang,
                    hd.thanhToanLamTron, hd.tienThua);
    }
}

void LichSuHoaDon::ghiHDCSV() {
    FILE* f = fopen("hoadon.csv", "wb");
    if (!f) return;
    fprintf(f, "\xEF\xBB\xBF"
        "MaHD;Ngay;Gio;NhanVien;KhachHang;SDT;HangKH;SoMatHang;"
        "TongTruocGiam;GiamSoLuong;GiamKH;TongSauGiam;"
        "ThanhToanLamTron;TienKhachDua;TienThua;DaHuy\r\n");
    for (const auto& h : dsHoaDon)
        fprintf(f, "%d;%s;%s;%s;%s;%s;%s;%d;%.0f;%.0f;%.0f;%.0f;%.0f;%.0f;%.0f;%d\r\n",
            h.maHD, h.ngay, h.gio,
            h.nhanVien.c_str(), h.tenKH.c_str(), h.sdtKH.c_str(),
            h.tenHangKH().data(), h.soMatHang,
            h.tongTruocGiam, h.giamSoLuong, h.giamKhachHang,
            h.tongSauGiam, h.thanhToanLamTron,
            h.tienKhachDua, h.tienThua, h.daHuy ? 1 : 0);
    fclose(f);
}

/* ================================================================
   GHI/DOC FILE KHACH HANG
================================================================ */
void LichSuHoaDon::ghiKHTXT() {
    ofstream f("khachhang.txt");
    if (!f) return;
    f << format("================================================================\n"
                "  NHA SACH FETEL | KHACH HANG | Tong: {}\n"
                "================================================================\n\n",
                dsKH.size());
    int i = 0;
    for (const auto& k : dsKH)
        f << format("[KH #{:04d}]\n"
                    "  SDT          : {}\n"
                    "  Ho ten       : {}\n"
                    "  Hang         : {}\n"
                    "  Tong don     : {} don\n"
                    "  Tong chi tieu: {:.0f} VND\n"
                    "  Diem tich luy: {} diem\n\n",
                    ++i, k.sdt, k.hoTen, k.tenHang(),
                    k.tongDonHang, k.tongChiTieu, k.diemTichLuy);
    f << "================================================================\n"
         "  Phan hang: Thuong|Than(200,5%)|VIP(500,10%)|VVIP(1500,15%)\n"
         "================================================================\n";
}

void LichSuHoaDon::ghiKHCSV() {
    FILE* f = fopen("khachhang.csv", "wb");
    if (!f) return;
    fprintf(f, "\xEF\xBB\xBF"
        "SDT;HoTen;Hang;TongDon;TongChiTieu;DiemTichLuy\r\n");
    for (const auto& k : dsKH)
        fprintf(f, "%s;%s;%s;%d;%.0f;%d\r\n",
            k.sdt.c_str(), k.hoTen.c_str(), k.tenHang().data(),
            k.tongDonHang, k.tongChiTieu, k.diemTichLuy);
    fclose(f);
}

void LichSuHoaDon::docKHTXT() {
    ifstream f("khachhang.txt");
    if (!f) return;
    dsKH.clear();
    string dong;
    KhachHang k;
    bool daCoKH = false;
    while (getline(f, dong)) {
        dong = trimLS(dong);
        if (dong.size() >= 5 && dong.substr(0, 5) == "[KH #") {
            if (daCoKH) dsKH.push_back(k);
            k = KhachHang{};
            daCoKH = true;
            continue;
        }
        if (!daCoKH) continue;
        auto val = layGiaTriLS(dong);
        auto chk = [&](string_view pre) {
            return dong.size() >= pre.size() &&
                   dong.substr(0, pre.size()) == pre;
        };
        if      (chk("SDT          :")) k.sdt         = val;
        else if (chk("Ho ten       :")) k.hoTen       = val;
        else if (chk("Tong don     :")) k.tongDonHang = stoi(val);
        else if (chk("Tong chi tieu:")) k.tongChiTieu = stof(val);
        else if (chk("Diem tich luy:")) {
            k.diemTichLuy = stoi(val);
            k.capNhatHang();
        }
    }
    if (daCoKH) dsKH.push_back(k);
}

/* ================================================================
   INTERFACE CONG KHAI
================================================================ */
void LichSuHoaDon::khoiTao() {
    docHDBIN();
    docKHTXT();
}

void LichSuHoaDon::luuTatCa() {
    ghiHDBIN();
    ghiHDTXT();
    ghiHDCSV();
    ghiKHTXT();
    ghiKHCSV();
}

KhachHang* LichSuHoaDon::timKH(const string& sdt) {
    for (auto& k : dsKH)
        if (k.sdt == sdt) return &k;
    return nullptr;
}

void LichSuHoaDon::capNhatKH(const string& sdt,
                              const string& hoTen,
                              float chiTieu) {
    if (auto* kh = timKH(sdt); kh) {
        kh->themDon(chiTieu);
    } else {
        KhachHang k;
        k.sdt   = sdt;
        k.hoTen = hoTen;
        k.themDon(chiTieu);
        dsKH.push_back(k);
    }
}

void LichSuHoaDon::themHoaDon(const HoaDon& hd) {
    dsHoaDon.push_back(hd);
    luuTatCa();
}