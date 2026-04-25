#pragma once
#include <vector>
#include <string>
#include <string_view>
#include <memory>

/* ================================================================ HANG SO */
enum class LoaiHang : int {
    Sach=1, VoPhanViet=2, DoChoi=3, LuuNiem=4, VanPhongPham=5
};

/* ================================================================
   CLASS HANGHOA -- Abstract Base Class
   Encapsulation + Abstraction (pure = 0 da duoc bo de don gian)
   Polymorphism: tenLoai, tacGia, theLoai, chatLieu, lopTu...
================================================================ */
class HangHoa {
public:
    int ma{0};
    std::string ten{}, nhaCungCap{};
    float giaNhap{0}, giaBan{0};
    int soLuongTon{0};
    LoaiHang loai{LoaiHang::VanPhongPham};
    bool ngungBan{false};

    HangHoa() = default;
    virtual ~HangHoa() = default;
    HangHoa(const HangHoa&) = default;
    HangHoa& operator=(const HangHoa&) = default;
    HangHoa(HangHoa&&) = default;
    HangHoa& operator=(HangHoa&&) = default;

    /* VIRTUAL FUNCTIONS - Polymorphism */
    [[nodiscard]] virtual std::string tenLoai()  const { return "Hang hoa"; }
    [[nodiscard]] virtual std::string tacGia()   const { return ""; }
    [[nodiscard]] virtual std::string theLoai()  const { return ""; }
    [[nodiscard]] virtual std::string chatLieu() const { return ""; }
    [[nodiscard]] virtual int lopTu()  const noexcept { return 0; }
    [[nodiscard]] virtual int lopDen() const noexcept { return 0; }
    [[nodiscard]] virtual int doTuoi() const noexcept { return 0; }
    [[nodiscard]] virtual int maLoai() const noexcept { return static_cast<int>(loai); }

    /* Tinh giam gia theo so luong - dung chung */
    [[nodiscard]] virtual float tinhGiamGia(int sl) const noexcept {
        if(sl>=25) return 0.20f;
        if(sl>=10) return 0.15f;
        if(sl>= 5) return 0.10f;
        if(sl>= 3) return 0.05f;
        return 0.0f;
    }
};

/* ================================================================
   CAC LOP CON -- Inheritance + Polymorphism (override)
================================================================ */
class Sach final : public HangHoa {
public:
    std::string _tacGia{}, _theLoai{};
    Sach() { loai = LoaiHang::Sach; }
    [[nodiscard]] std::string tenLoai() const override { return "Sach"; }
    [[nodiscard]] std::string tacGia()  const override { return _tacGia; }
    [[nodiscard]] std::string theLoai() const override { return _theLoai; }
};

class VoPhanViet final : public HangHoa {
public:
    int _lopTu{1}, _lopDen{5};
    VoPhanViet() { loai = LoaiHang::VoPhanViet; }
    [[nodiscard]] std::string tenLoai() const override { return "Vo/But/VPP"; }
    [[nodiscard]] int lopTu()  const noexcept override { return _lopTu; }
    [[nodiscard]] int lopDen() const noexcept override { return _lopDen; }
};

class DoChoi final : public HangHoa {
public:
    int _doTuoi{3};
    DoChoi() { loai = LoaiHang::DoChoi; }
    [[nodiscard]] std::string tenLoai() const override { return "Do choi"; }
    [[nodiscard]] int doTuoi() const noexcept override { return _doTuoi; }
};

class DoLuuNiem final : public HangHoa {
public:
    std::string _chatLieu{};
    DoLuuNiem() { loai = LoaiHang::LuuNiem; }
    [[nodiscard]] std::string tenLoai()  const override { return "Do luu niem"; }
    [[nodiscard]] std::string chatLieu() const override { return _chatLieu; }
};

class VanPhongPham final : public HangHoa {
public:
    VanPhongPham() { loai = LoaiHang::VanPhongPham; }
    [[nodiscard]] std::string tenLoai() const override { return "Van phong pham"; }
};

/* Factory function */
[[nodiscard]] std::unique_ptr<HangHoa> taoHangHoa(LoaiHang l);

/* ================================================================
   CLASS QUANLYKHO -- Encapsulation (du lieu + logic, KHONG co UI)
   Du lieu public de BookstoreApp (ImGui) truy cap truc tiep
================================================================ */
class QuanLyKho {
public:
    /* --- Du lieu (public de ImGui render) --- */
    std::vector<std::unique_ptr<HangHoa>> kho{};
    int maHangTiep{1001};

    QuanLyKho() = default;
    ~QuanLyKho() = default;
    QuanLyKho(const QuanLyKho&) = delete;
    QuanLyKho& operator=(const QuanLyKho&) = delete;

    /* --- Interface cong khai --- */
    void khoiTao();   // doc file, neu trong thi nap mau
    void luu();       // ghi ca TXT va CSV

    /* Them mat hang moi */
    void them(std::string_view ten, std::string_view ncc,
              float gN, float gB, int sl, LoaiHang l,
              std::string_view extra1 = "",
              std::string_view extra2 = "",
              int n1 = 0, int n2 = 0, int n3 = 0);

    /* Nhap them so luong vao hang da co (tim theo ma) */
    bool nhapThem(int ma, int sl);

    /* Bat/tat trang thai ngung ban */
    bool doiTrangThai(int ma, bool ngungBan);

private:
    void ghiCSV();
    void ghiTXT();
    void docTXT();
    void khoiTaoDuLieuMau();
};