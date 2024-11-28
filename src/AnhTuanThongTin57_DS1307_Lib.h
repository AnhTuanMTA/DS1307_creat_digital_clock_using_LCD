/*
  Thư viện cho DS1307 RTC
  Tác giả: Anh Tuan Thong Tin 57
  Ngày: 28/08/2024
  Cập nhật: 25/09
*/

#ifndef _ANHTUANTHONGTIN57_DS1307_H_
#define _ANHTUANTHONGTIN57_DS1307_H_
#include <Arduino.h>
#include <Wire.h>

class AnhTuanThongTin57_DS1307_Lib {
public:
  AnhTuanThongTin57_DS1307_Lib();
  AnhTuanThongTin57_DS1307_Lib(uint8_t diaChi);

  char batDau();
  uint8_t chuyenDoi(uint8_t duLieu);
  uint8_t chuyenDoiBCD(int duLieu);
  char layThoiGian();

  uint8_t layGiay();           // Trả về giây
  uint8_t layPhut();           // Trả về phút
  uint8_t layGio();            // Trả về giờ
  uint8_t layNgayTrongTuan();  // Trả về ngày trong tuần
  uint8_t layNgay();           // Trả về ngày
  uint8_t layThang();          // Trả về tháng
  uint8_t layNam();            // Trả về năm
  uint8_t layAMPM();           // Trả về 0=AM, 1=PM
  uint8_t layDinhDang();       // Trả về định dạng 12 hoặc 24 giờ

  void datDinhDang(uint8_t dinhDang);   // Đặt định dạng thời gian 12 hoặc 24
  void datAMPM(uint8_t ampm);           // Đặt AM hoặc PM: PM=1, AM=0
  void datThoiGian(const char *thoiGian); // Đặt thời gian Giờ:Phút:Giây
  void datNgay(const char *ngayThangNam); // Đặt ngày Tháng:Ngày:Năm
  void datNgayTrongTuan(uint8_t ngay);    // Đặt ngày trong tuần

private:
  uint8_t _diaChi;
  uint8_t _duLieu[13];

  uint8_t _phut;         // 0 - 60 phút
  uint8_t _giay;         // 0 - 60 giây
  uint8_t _gio;          // 1-12 hoặc 0 - 23 giờ
  uint8_t _ampm;         // 0=AM, 1=PM
  uint8_t _gioDinhDang;  // Định dạng 12 hoặc 24 giờ

  uint8_t _ngayTrongTuan; // 1-7 ngày trong tuần
  uint8_t _ngay;          // 1-31 ngày
  uint8_t _thang;         // 1-12 tháng
  uint8_t _nam;           // 0-99 năm
};

#endif
