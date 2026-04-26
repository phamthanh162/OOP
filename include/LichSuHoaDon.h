#pragma once
#include <string>
#include <string_view>
#include <vector>

constexpr int MAX_CHITIET = 20;

/* ================================================================
   ENUM HANG KHACH HANG
================================================================ */
enum class HangKH : int {
    Thuong = 0,
    Than   = 1,   // >= 200 diem, giam 5%
    VIP    = 2,   // >= 500 diem, giam 10%
    VVIP   = 3    // >= 1500 diem, giam 15%
};

/* ================================================================
   CLASS KHACH HANG -- Encapsulation
================================================================ */
class KhachHang {
public:
    std::string sdt{}, hoTen{};
    int   tongDonHang{0}, diemTichLuy{0};
    HangKH hang{HangKH::Thuong};
    float tongChiTieu{0.0f};

    KhachHang() = default;

    void capNhatHang() noexcept;
    void themDon(float chiTieu);

    [[nodiscard]] std::string_view tenHang()  const noexcept;
    [[nodiscard]] float            tyLeGiam() const noexcept;
};

/* ================================================================
   STRUCT CHI TIET HOA DON
================================================================ */
struct ChiTietHoaDon {
    int    maHang{0}, soLuong{0};
    float  giaBan{0}, giamGiaPhanTram{0}, thanhTien{0};
    std::string tenHang{};
};

/* ================================================================
   CLASS HOA DON -- Encapsulation + methods
================================================================ */
class HoaDon {
public:
    int    maHD{0}, hangKH{0}, soMatHang{0};
    char   ngay[12]{}, gio[9]{};
    std::string nhanVien{}, tenKH{}, sdtKH{};
    ChiTietHoaDon chiTiet[MAX_CHITIET]{};
    float  tongTruocGiam{0}, giamSoLuong{0}, giamKhachHang{0};
    float  tongSauGiam{0}, thanhToanLamTron{0};
    float  tienKhachDua{0}, tienThua{0};
    bool   daHuy{false};
    unsigned int checksum{0};

    HoaDon() = default;

    [[nodiscard]] unsigned int tinhChecksum() const noexcept;
    [[nodiscard]] std::string_view tenHangKH()  const noexcept;
    [[nodiscard]] float            tyLeGiamKH() const noexcept;
};

/* ================================================================
   BINARY STRUCT (khong co virtual pointer) -- de ghi file .dat
================================================================ */
#pragma pack(push, 1)
struct HoaDonBin {
    int   maHD, hangKH, soMatHang, daHuy;
    char  ngay[12], gio[9], nhanVien[60], tenKH[80], sdtKH[15];
    float tongTruocGiam, giamSoLuong, giamKhachHang;
    float tongSauGiam, thanhToanLamTron, tienKhachDua, tienThua;
    unsigned int checksum;
    struct {
        int   maHang, soLuong;
        float giaBan, giamGiaPhanTram, thanhTien;
        char  tenHang[100];
    } ct[MAX_CHITIET];
};
#pragma pack(pop)

/* ================================================================
   CLASS LICHSUHOADON -- Encapsulation toan bo lich su + khach hang
   Du lieu public de BookstoreApp (ImGui) truy cap truc tiep
================================================================ */
class LichSuHoaDon {
public:
    /* --- Du lieu (public de ImGui render) --- */
    std::vector<HoaDon>    dsHoaDon{};
    std::vector<KhachHang> dsKH{};
    int maHDTiep{1};

    LichSuHoaDon() = default;
    ~LichSuHoaDon() = default;
    LichSuHoaDon(const LichSuHoaDon&) = delete;
    LichSuHoaDon& operator=(const LichSuHoaDon&) = delete;

    /* --- Interface cong khai --- */
    void khoiTao();    // doc file .dat, .txt
    void luuTatCa();   // ghi tat ca file

    /* Tim khach hang theo SDT */
    [[nodiscard]] KhachHang* timKH(const std::string& sdt);

    /* Cap nhat hoac them moi khach hang sau khi mua */
    void capNhatKH(const std::string& sdt,
                   const std::string& hoTen,
                   float chiTieu);

    /* Them hoa don moi (da tinh toan xong) */
    void themHoaDon(const HoaDon& hd);

private:
    /* File hoa don (binary, chong sua) */
    void ghiHDBIN();
    void docHDBIN();
    void ghiHDCSV();
    void ghiHDTXT();

    /* File khach hang */
    void ghiKHTXT();
    void ghiKHCSV();
    void docKHTXT();
};