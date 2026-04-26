#pragma once
#include "Khohang.h"
#include "LichSuHoaDon.h"
#include <string>

class BookstoreApp {
public:
    BookstoreApp();
    void run();

private:
    QuanLyKho    kho;
    LichSuHoaDon lichSu;

    bool showBanHang    = false;
    bool showKho        = false;
    bool showLichSu     = false;
    bool showKhachHang  = false;
    bool showBaoCao     = false;
    bool showFormThem   = false;
    bool showFormNhap   = false;

    ChiTietHoaDon hdChiTiet[MAX_CHITIET]{};
    int   hdSoMatHang    = 0;
    float hdTongTruocGiam= 0;
    float hdGiamSoLuong  = 0;
    int   hdHangKH       = 0;

    HangHoa* hangDangChon = nullptr;
    HangHoa* hangDangSua  = nullptr;
    HoaDon*  hdDangXem    = nullptr;

    char buf_ten[128]      = {};
    char buf_ncc[128]      = {};
    char buf_gia_nhap[32]  = {};
    char buf_gia_ban[32]   = {};
    char buf_sl[16]        = {};
    char buf_extra1[128]   = {};
    char buf_extra2[128]   = {};
    char buf_sdt[20]       = {};
    char buf_ten_kh[100]   = {};
    char buf_tim[128]      = {};
    int  loaiChon          = 0;

    std::string thongBao   = "";

    void renderMenuChinh();
    void renderBanHang();
    void renderQuanLyKho();
    void renderLichSuHoaDon();
    void renderKhachHang();
    void renderBaoCaoNgay();
    void resetBuf();
};