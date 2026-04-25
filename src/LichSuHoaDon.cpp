/*
  NHA SACH FETEL - HCMUS - 23DTV_CLC1
  MODULE: LICH SU HOA DON (chay doc lap)
  C++23 | Compiler: -std=c++23 -lm
  Luu y: doc file hoadon.dat (tao boi chuong trinh chinh)
         Neu chua co file, hien thi "Chua co hoa don"
*/
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <string_view>
#include <ranges>
#include <format>
#include <array>
#include <cstring>
#include <cstdio>
#include <windows.h>
#include <conio.h>
using namespace std;

/* ================================================================ HANG SO */
constexpr int MAX_CHITIET=20;
constexpr string_view FILE_HD_BIN="hoadon.dat";
constexpr string_view FILE_HD_TXT="hoadon.txt";
constexpr string_view FILE_HD_CSV="hoadon.csv";
enum class HangKH:int{Thuong=0,Than=1,VIP=2,VVIP=3};
constexpr int MAU_TRANG=7,MAU_XANH_SANG=11,MAU_VANG=14,MAU_DO=12,MAU_TIM=13;
constexpr int MAU_XANH_LA=10,MAU_XANH_DUONG=9,MAU_XAM=8,MAU_TRANG_SANG=15;
constexpr int MAU_DEN_XANH=0*16+11,MAU_XANH_TRANG=1*16+15;
constexpr int PHIM_LEN=72,PHIM_XUONG=80,PHIM_ENTER=13,PHIM_ESC=27;

/* ================================================================ CONSOLE */
static HANDLE hCon;
static void setMau(int m) noexcept{SetConsoleTextAttribute(hCon,m);}
static void cls()          noexcept{system("cls");}
static void gotoxy(int x,int y) noexcept{COORD c{(SHORT)x,(SHORT)y};SetConsoleCursorPosition(hCon,c);}
static void anConTro(bool an) noexcept{
    CONSOLE_CURSOR_INFO ci;GetConsoleCursorInfo(hCon,&ci);
    ci.bVisible=an?FALSE:TRUE;SetConsoleCursorInfo(hCon,&ci);
}
static void setKichThuoc(int w,int h){
    system(format("mode con: cols={} lines={}",w,h).c_str());
    SMALL_RECT wr{0,0,(SHORT)(w-1),(SHORT)(h-1)};COORD bs{(SHORT)w,(SHORT)(h+200)};
    SetConsoleScreenBufferSize(hCon,bs);SetConsoleWindowInfo(hCon,TRUE,&wr);
}
static void veHeader(){
    setMau(MAU_XANH_TRANG);gotoxy(0,0);printf("%-82s","");gotoxy(0,0);
    printf(" NHA SACH FETEL - HCMUS - 23DTV_CLC1  |  235 Nguyen Van Cu, Q5, TP.HCM  | 23207022.19.14");
    setMau(MAU_XANH_SANG);gotoxy(0,1);for(int i=0;i<82;i++)printf("=");setMau(MAU_TRANG);
}
static void veFooter(string_view s){
    setMau(MAU_XAM);gotoxy(0,28);for(int i=0;i<82;i++)printf("-");
    gotoxy(0,29);printf("  %s",s.data());setMau(MAU_TRANG);
}
static void nhanPhimBatKy(){
    setMau(MAU_XAM);gotoxy(2,27);printf("[ Nhan phim bat ky de quay lai... ]");
    setMau(MAU_TRANG);getch();
}
static string trimStr(string_view s){
    auto a=s.find_first_not_of(" \t\r\n");if(a==string_view::npos)return"";
    return string(s.substr(a,s.find_last_not_of(" \t\r\n")-a+1));
}
static void clearInput(){int c;while((c=getchar())!='\n'&&c!=EOF);}
static void nhapStr(const char* msg,char* buf,int n){
    printf("%s",msg);setMau(MAU_VANG);if(fgets(buf,n,stdin))buf[strcspn(buf,"\n")]=0;setMau(MAU_TRANG);
}
[[nodiscard]] static int menuMuiTen(int x,int y,const vector<string_view>& items){
    int chon=0;anConTro(true);
    while(true){
        for(int i=0;i<(int)items.size();i++){
            gotoxy(x,y+i);
            if(i==chon){setMau(MAU_DEN_XANH);printf("  > %-42s",items[i].data());}
            else        {setMau(MAU_TRANG);   printf("    %-42s",items[i].data());}
        }
        setMau(MAU_TRANG);int p=getch();
        if(p==0||p==0xE0){p=getch();
            if(p==PHIM_LEN&&chon>0)chon--;
            if(p==PHIM_XUONG&&chon<(int)items.size()-1)chon++;
        }else if(p==PHIM_ENTER){anConTro(false);return chon;}
         else if(p==PHIM_ESC)  {anConTro(false);return -1;}
    }
}

/* ================================================================
   STRUCT CHI TIET + CLASS HOA DON
================================================================ */
struct ChiTietHoaDon{
    int maHang{0},soLuong{0};
    float giaBan{0},giamGiaPhanTram{0},thanhTien{0};
    string tenHang{};
};

class HoaDon{
public:
    int maHD{0},hangKH{0},soMatHang{0};
    char ngay[12]{},gio[9]{};
    string nhanVien{},tenKH{},sdtKH{};
    ChiTietHoaDon chiTiet[MAX_CHITIET]{};
    float tongTruocGiam{0},giamSoLuong{0},giamKhachHang{0};
    float tongSauGiam{0},thanhToanLamTron{0},tienKhachDua{0},tienThua{0};
    bool daHuy{false};unsigned int checksum{0};

    HoaDon()=default;

    [[nodiscard]] unsigned int tinhChecksum() const noexcept{
        unsigned int s=static_cast<unsigned>(maHD)
                      +static_cast<unsigned>(tongTruocGiam*100)
                      +static_cast<unsigned>(thanhToanLamTron*100);
        for(auto c:nhanVien)s+=static_cast<unsigned char>(c);
        for(int i=0;i<soMatHang;i++){
            s+=static_cast<unsigned>(chiTiet[i].thanhTien*100);
            s+=static_cast<unsigned>(chiTiet[i].soLuong);
        }
        return s^0xFE7E1123u;
    }
    [[nodiscard]] string_view tenHangKH() const noexcept{
        if(hangKH==static_cast<int>(HangKH::VVIP))return"VVIP";
        if(hangKH==static_cast<int>(HangKH::VIP)) return"VIP";
        if(hangKH==static_cast<int>(HangKH::Than))return"Than thiet";
        return"Thuong";
    }
    [[nodiscard]] float tyLeGiamKH() const noexcept{
        if(hangKH==static_cast<int>(HangKH::VVIP))return 0.15f;
        if(hangKH==static_cast<int>(HangKH::VIP)) return 0.10f;
        if(hangKH==static_cast<int>(HangKH::Than))return 0.05f;
        return 0.0f;
    }
    [[nodiscard]] int mauHangKH() const noexcept{
        if(hangKH==static_cast<int>(HangKH::VVIP))return MAU_DO;
        if(hangKH==static_cast<int>(HangKH::VIP)) return MAU_TIM;
        if(hangKH==static_cast<int>(HangKH::Than))return MAU_XANH_LA;
        return MAU_TRANG;
    }

    void in() const{
        cls();veHeader();
        setMau(MAU_VANG);
        printf("\n  +----------------------------------------------------+\n"
               "  |           HOA DON BAN HANG                        |\n"
               "  |    NHA SACH FETEL - HCMUS - 23DTV_CLC1            |\n"
               "  |    235 Nguyen Van Cu, Q5, TP.HCM                  |\n"
               "  +----------------------------------------------------+\n");
        setMau(MAU_TRANG);
        printf("  So HD: #%04d | Ngay: %s | Gio: %s\n",maHD,ngay,gio);
        printf("  Nhan vien: %s\n",nhanVien.c_str());
        printf("  Khach hang: %s | SDT: %s\n",tenKH.c_str(),sdtKH.c_str());
        setMau(mauHangKH());
        printf("  Hang KH: %s (giam %.0f%% tren tong)\n",tenHangKH().data(),tyLeGiamKH()*100);
        setMau(MAU_TRANG);
        if(daHuy){setMau(MAU_DO);printf("  *** HOA DON NAY DA BI HUY ***\n");setMau(MAU_TRANG);}
        setMau(MAU_XANH_SANG);
        printf("  %-4s %-26s %4s %9s %5s %9s\n","STT","Ten hang","SL","Don gia","Giam%","T.Tien");
        printf("  ----------------------------------------------------------\n");setMau(MAU_TRANG);
        for(int i=0;i<soMatHang;i++){
            const auto&ct=chiTiet[i];
            printf("  %-4d %-26s %4d %9.0f",i+1,ct.tenHang.c_str(),ct.soLuong,ct.giaBan);
            if(ct.giamGiaPhanTram>0){setMau(MAU_XANH_LA);printf(" %4.0f%%",ct.giamGiaPhanTram*100);setMau(MAU_TRANG);}
            else printf("     ");
            printf(" %9.0f\n",ct.thanhTien);
        }
        setMau(MAU_XANH_SANG);printf("  ----------------------------------------------------------\n");setMau(MAU_TRANG);
        printf("  Tong truoc giam   : %10.0f VND\n",tongTruocGiam);
        if(giamSoLuong>0){setMau(MAU_XANH_LA);printf("  Giam so luong     : %10.0f VND\n",giamSoLuong);setMau(MAU_TRANG);}
        if(giamKhachHang>0){setMau(MAU_XANH_LA);printf("  Giam hang KH(%.0f%%) : %10.0f VND\n",tyLeGiamKH()*100,giamKhachHang);setMau(MAU_TRANG);}
        printf("  Tong sau giam     : %10.0f VND\n",tongSauGiam);
        setMau(MAU_VANG);printf("  THANH TOAN (lt)   : %10.0f VND\n",thanhToanLamTron);setMau(MAU_TRANG);
        printf("  Tien khach dua    : %10.0f VND\n",tienKhachDua);
        setMau(MAU_XANH_LA);printf("  Tien thua         : %10.0f VND\n",tienThua);setMau(MAU_TRANG);
        printf("  Diem tich luy them: %d diem\n",static_cast<int>(thanhToanLamTron/10000));
        setMau(MAU_XAM);printf("\n       Cam on quy khach! Hen gap lai!\n");setMau(MAU_TRANG);
    }
};

/* Binary struct de doc file */
#pragma pack(push,1)
struct HoaDonBin{
    int maHD,hangKH,soMatHang,daHuy;
    char ngay[12],gio[9],nhanVien[60],tenKH[80],sdtKH[15];
    float tongTruocGiam,giamSoLuong,giamKhachHang,tongSauGiam;
    float thanhToanLamTron,tienKhachDua,tienThua;unsigned int checksum;
    struct{int maHang,soLuong;float giaBan,giamGiaPhanTram,thanhTien;char tenHang[100];}ct[MAX_CHITIET];
};
#pragma pack(pop)

/* ================================================================
   CLASS LICHSUHOADON -- Encapsulation toan bo chuc nang
================================================================ */
class LichSuHoaDon{
private:
    vector<HoaDon> ds{};
    int maHDTiep{1};

    /* --- doc/ghi file --- */
    void docBIN(){
        FILE*f=fopen(FILE_HD_BIN.data(),"rb");if(!f)return;
        ds.clear();int tong=0;
        fread(&tong,sizeof(int),1,f);fread(&maHDTiep,sizeof(int),1,f);
        for(int i=0;i<tong;i++){
            HoaDonBin b{};fread(&b,sizeof(HoaDonBin),1,f);
            HoaDon hd{};
            hd.maHD=b.maHD;hd.hangKH=b.hangKH;hd.soMatHang=b.soMatHang;hd.daHuy=(b.daHuy!=0);
            strncpy(hd.ngay,b.ngay,11);strncpy(hd.gio,b.gio,8);
            hd.nhanVien=b.nhanVien;hd.tenKH=b.tenKH;hd.sdtKH=b.sdtKH;
            hd.tongTruocGiam=b.tongTruocGiam;hd.giamSoLuong=b.giamSoLuong;
            hd.giamKhachHang=b.giamKhachHang;hd.tongSauGiam=b.tongSauGiam;
            hd.thanhToanLamTron=b.thanhToanLamTron;hd.tienKhachDua=b.tienKhachDua;hd.tienThua=b.tienThua;
            hd.checksum=b.checksum;
            for(int j=0;j<b.soMatHang&&j<MAX_CHITIET;j++){
                hd.chiTiet[j].maHang=b.ct[j].maHang;hd.chiTiet[j].soLuong=b.ct[j].soLuong;
                hd.chiTiet[j].giaBan=b.ct[j].giaBan;hd.chiTiet[j].giamGiaPhanTram=b.ct[j].giamGiaPhanTram;
                hd.chiTiet[j].thanhTien=b.ct[j].thanhTien;hd.chiTiet[j].tenHang=b.ct[j].tenHang;
            }
            /* Kiem tra checksum - loai bo neu bi chinh sua ben ngoai */
            if(hd.tinhChecksum()==hd.checksum) ds.push_back(hd);
        }
        fclose(f);
    }

    void ghiTXT(){
        ofstream f(FILE_HD_TXT.data());if(!f)return;
        f<<format("================================================================\n  NHA SACH FETEL | LICH SU HOA DON | Tong: {} don\n================================================================\n\n",ds.size());
        for(const auto&hd:ds){
            char buf[200];
            f<<"+----------------------------------------------------------+\n";
            sprintf(buf,"| HD #%04d  |  %s  %s  |  NV: %-20s |",hd.maHD,hd.ngay,hd.gio,hd.nhanVien.c_str());f<<buf<<"\n";
            sprintf(buf,"| Khach: %-20s SDT: %-13s Hang: %-6s|",hd.tenKH.c_str(),hd.sdtKH.c_str(),hd.tenHangKH().data());f<<buf<<"\n";
            f<<"+--------+-------------------------------+-----+-----------+\n| Ma     | Ten hang                      |  SL | Thanh tien|\n+--------+-------------------------------+-----+-----------+\n";
            for(int j=0;j<hd.soMatHang;j++){
                sprintf(buf,"| %-6d | %-29s | %3d | %9.0f |",hd.chiTiet[j].maHang,hd.chiTiet[j].tenHang.c_str(),hd.chiTiet[j].soLuong,hd.chiTiet[j].thanhTien);f<<buf<<"\n";
            }
            f<<format("+--------+-------------------------------+-----+-----------+\n  Tong: {:.0f}  Giam SL: {:.0f}  Giam KH: {:.0f}  THANH TOAN: {:.0f}\n\n",
                hd.tongTruocGiam,hd.giamSoLuong,hd.giamKhachHang,hd.thanhToanLamTron);
        }
    }

    void ghiCSV(){
        FILE*f=fopen(FILE_HD_CSV.data(),"wb");if(!f)return;
        fprintf(f,"\xEF\xBB\xBF""MaHD;Ngay;Gio;NhanVien;KhachHang;SDT;HangKH;SoMatHang;TongTruocGiam;GiamSoLuong;GiamKH;TongSauGiam;ThanhToanLamTron;TienKhachDua;TienThua;DaHuy\r\n");
        for(const auto&h:ds)
            fprintf(f,"%d;%s;%s;%s;%s;%s;%s;%d;%.0f;%.0f;%.0f;%.0f;%.0f;%.0f;%.0f;%d\r\n",
                h.maHD,h.ngay,h.gio,h.nhanVien.c_str(),h.tenKH.c_str(),h.sdtKH.c_str(),
                h.tenHangKH().data(),h.soMatHang,h.tongTruocGiam,h.giamSoLuong,h.giamKhachHang,
                h.tongSauGiam,h.thanhToanLamTron,h.tienKhachDua,h.tienThua,h.daHuy?1:0);
        fclose(f);
    }

    /* --- cac chuc nang hien thi --- */
    void xemDanhSach(){
        int trang=0,tongTrang=1;
        while(true){
            cls();veHeader();setMau(MAU_VANG);gotoxy(2,2);
            printf("=== LICH SU HOA DON (%d don) ===",(int)ds.size());setMau(MAU_TRANG);
            tongTrang=((int)ds.size()+17)/18;if(tongTrang==0)tongTrang=1;
            gotoxy(2,4);setMau(MAU_XANH_SANG);
            printf("%-6s %-12s %-8s %-18s %-16s %12s","SO HD","NGAY","GIO","NHAN VIEN","KHACH","THANH TOAN");setMau(MAU_TRANG);
            int bat=trang*18,dong=5;
            for(int i=bat;i<bat+18&&i<(int)ds.size();i++){
                const auto&h=ds[i];gotoxy(2,dong);if(h.daHuy)setMau(MAU_DO);
                printf("#%-5d %-12s %-8s %-18s %-16s %12.0f%s",
                    h.maHD,h.ngay,h.gio,h.nhanVien.c_str(),h.tenKH.c_str(),h.thanhToanLamTron,h.daHuy?" [HUY]":"");
                setMau(MAU_TRANG);dong++;
            }
            gotoxy(2,25);setMau(MAU_XAM);
            printf("  Trang %d/%d | P=Trang sau | O=Trang truoc | So HD=Xem chi tiet | Q=Quay lai",trang+1,tongTrang);setMau(MAU_TRANG);
            gotoxy(2,26);char inp[20];nhapStr("  Nhap: ",inp,20);auto s=trimStr(string(inp));
            if(s=="P"||s=="p"){if(trang<tongTrang-1)trang++;}
            else if(s=="O"||s=="o"){if(trang>0)trang--;}
            else if(s=="Q"||s=="q"||s.empty())break;
            else{
                int ma=s.empty()?0:stoi(s);
                auto it=ranges::find_if(ds,[ma](const auto&h){return h.maHD==ma;});
                if(it!=ds.end()){it->in();nhanPhimBatKy();}
                else{gotoxy(2,27);setMau(MAU_DO);printf("  Khong tim thay hoa don #%d!",ma);setMau(MAU_TRANG);Sleep(800);}
            }
        }
    }

    void timKiemHoaDon(){
        cls();veHeader();setMau(MAU_VANG);gotoxy(2,2);printf("=== TIM KIEM HOA DON ===");setMau(MAU_TRANG);
        gotoxy(2,4);printf("  Tim theo:\n");
        gotoxy(2,5);printf("  1. So hoa don (Ma HD)\n");
        gotoxy(2,6);printf("  2. Ten khach hang\n");
        gotoxy(2,7);printf("  3. Ten nhan vien\n");
        gotoxy(2,8);printf("  4. Ngay (dd/mm/yyyy)\n");
        gotoxy(2,9);
        printf("  Chon (1-4): ");setMau(MAU_VANG);int lua=0;scanf("%d",&lua);clearInput();setMau(MAU_TRANG);
        char tuKhoa[80];
        gotoxy(2,11);nhapStr("  Nhap tu khoa tim: ",tuKhoa,80);
        string tk=trimStr(string(tuKhoa));
        vector<const HoaDon*> ketQua;
        for(const auto&hd:ds){
            bool hop=false;
            switch(lua){
                case 1:hop=(to_string(hd.maHD).find(tk)!=string::npos);break;
                case 2:hop=(string(hd.tenKH).find(tk)!=string::npos);break;
                case 3:hop=(hd.nhanVien.find(tk)!=string::npos);break;
                case 4:hop=(strcmp(hd.ngay,tuKhoa)==0);break;
            }
            if(hop)ketQua.push_back(&hd);
        }
        cls();veHeader();setMau(MAU_VANG);gotoxy(2,2);
        printf("=== KET QUA TIM KIEM: %d hoa don ===",(int)ketQua.size());setMau(MAU_TRANG);
        if(ketQua.empty()){gotoxy(2,4);setMau(MAU_DO);printf("  Khong tim thay ket qua nao.");setMau(MAU_TRANG);nhanPhimBatKy();return;}
        gotoxy(2,4);setMau(MAU_XANH_SANG);
        printf("%-6s %-12s %-18s %-16s %12s","SO HD","NGAY","NHAN VIEN","KHACH","THANH TOAN");setMau(MAU_TRANG);
        int dong=5;
        for(const auto*h:ketQua){
            gotoxy(2,dong);if(h->daHuy)setMau(MAU_DO);
            printf("#%-5d %-12s %-18s %-16s %12.0f%s",h->maHD,h->ngay,h->nhanVien.c_str(),h->tenKH.c_str(),h->thanhToanLamTron,h->daHuy?" [HUY]":"");
            setMau(MAU_TRANG);dong++;
            if(dong>24)break;
        }
        gotoxy(2,26);char inp[20];nhapStr("  Nhap so HD de xem chi tiet (0=bo qua): ",inp,20);
        int ma=atoi(inp);
        if(ma>0){
            auto it=ranges::find_if(ds,[ma](const auto&h){return h.maHD==ma;});
            if(it!=ds.end()){it->in();nhanPhimBatKy();}
        }
        nhanPhimBatKy();
    }

    void thongKe(){
        cls();veHeader();setMau(MAU_VANG);gotoxy(2,2);printf("=== THONG KE HOA DON ===");setMau(MAU_TRANG);
        if(ds.empty()){gotoxy(2,4);setMau(MAU_XAM);printf("  Chua co hoa don nao.");setMau(MAU_TRANG);nhanPhimBatKy();return;}
        int tongHD=(int)ds.size();
        int soHuy=(int)ranges::count_if(ds,[](const auto&h){return h.daHuy;});
        float tongDT=0,tongGiamSL=0,tongGiamKH=0;
        for(const auto&h:ds)if(!h.daHuy){tongDT+=h.thanhToanLamTron;tongGiamSL+=h.giamSoLuong;tongGiamKH+=h.giamKhachHang;}
        /* Thong ke theo nhan vien */
        map<string,pair<int,float>> nvStat;
        for(const auto&h:ds)if(!h.daHuy){nvStat[h.nhanVien].first++;nvStat[h.nhanVien].second+=h.thanhToanLamTron;}
        /* Thong ke theo hang KH */
        int soThuong=0,soThan=0,soVIP=0,soVVIP=0;
        for(const auto&h:ds){
            if(h.hangKH==static_cast<int>(HangKH::VVIP))soVVIP++;
            else if(h.hangKH==static_cast<int>(HangKH::VIP))soVIP++;
            else if(h.hangKH==static_cast<int>(HangKH::Than))soThan++;
            else soThuong++;
        }
        gotoxy(2,4);setMau(MAU_XANH_SANG);printf("  +--- TONG QUAN -------------------------------------------+");setMau(MAU_TRANG);
        gotoxy(2,5); printf("  Tong hoa don       : %d",tongHD);
        gotoxy(2,6); printf("  Hoa don thanh cong : %d",tongHD-soHuy);
        gotoxy(2,7); printf("  Hoa don da huy     : %d",soHuy);
        setMau(MAU_VANG);gotoxy(2,8);printf("  Tong doanh thu     : %.0f VND",tongDT);setMau(MAU_TRANG);
        setMau(MAU_XANH_LA);gotoxy(2,9);printf("  Tong giam so luong : %.0f VND",tongGiamSL);setMau(MAU_TRANG);
        setMau(MAU_XANH_LA);gotoxy(2,10);printf("  Tong giam hang KH  : %.0f VND",tongGiamKH);setMau(MAU_TRANG);
        gotoxy(2,12);setMau(MAU_XANH_SANG);printf("  +--- PHAN LOAI KHACH HANG --------------------------------+");setMau(MAU_TRANG);
        gotoxy(2,13);printf("  Khach thuong   : %d don",soThuong);
        setMau(MAU_XANH_LA);gotoxy(2,14);printf("  Khach than thiet: %d don",soThan);setMau(MAU_TRANG);
        setMau(MAU_TIM);    gotoxy(2,15);printf("  Khach VIP       : %d don",soVIP);setMau(MAU_TRANG);
        setMau(MAU_DO);     gotoxy(2,16);printf("  Khach VVIP      : %d don",soVVIP);setMau(MAU_TRANG);
        gotoxy(2,18);setMau(MAU_XANH_SANG);printf("  +--- NHAN VIEN -------------------------------------------+");setMau(MAU_TRANG);
        int dong=19;
        for(const auto&[nv,stat]:nvStat){
            if(dong>25)break;
            gotoxy(2,dong);if(stat.first>0)setMau(MAU_XANH_LA);
            printf("  %-20s: %3d don | %.0f VND",nv.c_str(),stat.first,stat.second);
            setMau(MAU_TRANG);dong++;
        }
        nhanPhimBatKy();
    }

    void xuatFile(){
        cls();veHeader();setMau(MAU_VANG);gotoxy(2,2);printf("=== XUAT FILE ===");setMau(MAU_TRANG);
        gotoxy(2,4);printf("  Dang xuat...\n");
        ghiTXT();ghiCSV();
        gotoxy(2,6);setMau(MAU_XANH_LA);
        printf("  Da xuat:\n");
        printf("  - %s\n",FILE_HD_TXT.data());
        printf("  - %s\n",FILE_HD_CSV.data());
        setMau(MAU_TRANG);
        nhanPhimBatKy();
    }

public:
    LichSuHoaDon()=default;
    ~LichSuHoaDon()=default;
    LichSuHoaDon(const LichSuHoaDon&)=delete;
    LichSuHoaDon& operator=(const LichSuHoaDon&)=delete;

    void khoiTao(){
        docBIN();
        if(ds.empty()){
            /* Khong co file hoac chua co hoa don - khong sao, hien danh sach rong */
        }
    }

    void chay(){
        const vector<string_view> menu={
            "Xem danh sach hoa don","Tim kiem hoa don",
            "Thong ke tong hop","Xuat file TXT va CSV",
            "Tai lai tu file","Thoat"
        };
        while(true){
            cls();veHeader();
            setMau(MAU_VANG);gotoxy(28,3);printf("LICH SU HOA DON");setMau(MAU_TRANG);
            gotoxy(2,4);setMau(MAU_XAM);
            printf("  NHA SACH FETEL | %d hoa don | Doc tu: %s",(int)ds.size(),FILE_HD_BIN.data());setMau(MAU_TRANG);
            veFooter("Phim LEN/XUONG de chon  |  ENTER xac nhan  |  ESC = Thoat");
            int c=menuMuiTen(18,6,menu);
            if(c==-1||c==5){
                cls();setMau(MAU_XANH_LA);gotoxy(20,12);printf("  Da thoat Lich Su Hoa Don. Tam biet!");
                setMau(MAU_TRANG);Sleep(1200);break;
            }
            switch(c){
                case 0:xemDanhSach();break;   case 1:timKiemHoaDon();break;
                case 2:thongKe();break;        case 3:xuatFile();break;
                case 4:docBIN();cls();veHeader();setMau(MAU_XANH_LA);gotoxy(2,4);printf("  Da tai lai: %d hoa don.",(int)ds.size());setMau(MAU_TRANG);Sleep(1000);break;
            }
        }
    }
};

/* Them include map cho thong ke nhan vien */
#include <map>

/* ================================================================ MAIN */
int main(){
    hCon=GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTitle("Lich Su Hoa Don - Nha Sach Fetel [C++23]");
    SetConsoleOutputCP(1258);SetConsoleCP(1258);
    setKichThuoc(82,32);anConTro(true);

    LichSuHoaDon lshd{};
    lshd.khoiTao();
    lshd.chay();

    anConTro(false);return 0;
}