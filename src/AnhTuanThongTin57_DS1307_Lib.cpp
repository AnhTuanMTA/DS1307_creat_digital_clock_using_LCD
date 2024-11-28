/*
  Thư viện cho DS1307 RTC
  Tác giả: Anh Tuan Thong Tin 57
  Ngày: 28/08/2024
  Cập nhật: 25/09
*/

#include "AnhTuanThongTin57_DS1307_Lib.h"

AnhTuanThongTin57_DS1307_Lib::AnhTuanThongTin57_DS1307_Lib() {}
AnhTuanThongTin57_DS1307_Lib::AnhTuanThongTin57_DS1307_Lib(uint8_t diaChi) {
  _address = diaChi;
}

char AnhTuanThongTin57_DS1307_Lib::batDau() {
	Wire.begin();
	Wire.beginTransmission(_address);
    Wire.write(0x00);
    Wire.endTransmission();

    int i = 0;
    int trangThai = (uint8_t)Wire.requestFrom((int)_address, 8);
    while (Wire.available()) {
      _data[i] = Wire.read();
      i++;
    }

    if (_data[0] & 0x80) {
      Wire.beginTransmission(_address);
      Wire.write(0x00);
      Wire.write(0x7F & _data[0]);
      Wire.endTransmission();
    }

    if (trangThai) {
        return trangThai;
    } else {
        return 0;
    }
}

uint8_t AnhTuanThongTin57_DS1307_Lib::chuyenDoi(uint8_t Data) {
  uint8_t Temp_conv = Data & 0x0f;
  Temp_conv += ((Data & 0xf0) >> 4) * 10;
  return Temp_conv;
}

uint8_t AnhTuanThongTin57_DS1307_Lib::chuyenDoiBCD(int Data) {
  int tmp1;
  if (Data > 10) {
    tmp1 = (int)(Data / 10);
  } else {
    tmp1 = 0;
  }
  int tmp2 = Data - (tmp1 * 10);

  uint8_t Temp_conv = (tmp1 << 4) | tmp2;

  return Temp_conv;
}

char AnhTuanThongTin57_DS1307_Lib::layThoiGian() {
  Wire.beginTransmission(_address);
  Wire.write(0x00);
  Wire.endTransmission();

  int i = 0;
  int trangThai = (uint8_t)Wire.requestFrom((int)_address, 8);
  while (Wire.available()) {
    _data[i] = Wire.read();
    i++;
  }

  _giay = chuyenDoi(_data[0]);  // 0 - 60 giây
  _phut = chuyenDoi(_data[1]);  // 0 - 60 phút

  if (_data[2] & 0x40) {
    _gio = chuyenDoi(_data[2] & 0x1F);
    _ampm = (_data[2] & 0x20) >> 5;
    _ampm = (~_ampm) & 1;
    _gioDinhDang = 12;
  } else {
    _gio = chuyenDoi(_data[2] & 0x3F);
    _gioDinhDang = 24;
  }

  _ngayTrongTuan = _data[3];           // 1-7 ngày trong tuần
  _ngay = chuyenDoi(_data[4]);         // 1-31 ngày
  _thang = chuyenDoi(_data[5] & 0x3f); // 1-12 tháng
  _nam = chuyenDoi(_data[6]);          // 0-99 năm

  return trangThai;
}

uint8_t AnhTuanThongTin57_DS1307_Lib::layGiay() { return _giay; }
uint8_t AnhTuanThongTin57_DS1307_Lib::layPhut() { return _phut; }
uint8_t AnhTuanThongTin57_DS1307_Lib::layGio() { return _gio; }
uint8_t AnhTuanThongTin57_DS1307_Lib::layNgayTrongTuan() { return _ngayTrongTuan; }
uint8_t AnhTuanThongTin57_DS1307_Lib::layNgay() { return _ngay; }
uint8_t AnhTuanThongTin57_DS1307_Lib::layThang() { return _thang; }
uint8_t AnhTuanThongTin57_DS1307_Lib::layNam() { return _nam; }
uint8_t AnhTuanThongTin57_DS1307_Lib::layAMPM() { return _ampm; }
uint8_t AnhTuanThongTin57_DS1307_Lib::layDinhDang() { return _gioDinhDang; }

void AnhTuanThongTin57_DS1307_Lib::datDinhDang(uint8_t dinhDang) {
    layThoiGian();

  if (dinhDang == 12) {
    _data[2] |= 0x40;
    Wire.beginTransmission(_address);
    Wire.write(0x02);
    Wire.write(_data[2]);
    Wire.endTransmission();
    _gioDinhDang = 12;
  } else {
    _data[2] &= 0x3f;
    Wire.beginTransmission(_address);
    Wire.write(0x02);
    Wire.write(_data[2]);
    Wire.endTransmission();
    _gioDinhDang = 24;
  }
}

void AnhTuanThongTin57_DS1307_Lib::datAMPM(uint8_t ampm) {
    layThoiGian();

  if (ampm == 1) {
    _data[2] |= 0x20;
    Wire.beginTransmission(_address);
    Wire.write(0x02);
    Wire.write(_data[2]);
    Wire.endTransmission();
    _ampm = 1;
  } else {
    _data[2] &= 0x5f;
    Wire.beginTransmission(_address);
    Wire.write(0x02);
    Wire.write(_data[2]);
    Wire.endTransmission();
    _ampm = 0;
  }
}

void AnhTuanThongTin57_DS1307_Lib::datThoiGian(const char *thoiGian) {
  int giay;
  int phut;
  int gio;
  sscanf(thoiGian, "%d:%d:%d", &gio, &phut, &giay);

  _data[0] = chuyenDoiBCD(giay);
  _data[1] = chuyenDoiBCD(phut);
  _data[2] &= 0x60;
  _data[2] |= chuyenDoiBCD(gio);

  Wire.beginTransmission(_address);
  Wire.write(0x00);
  Wire.write(_data[0]);
  Wire.write(_data[1]);
  Wire.write(_data[2]);
  Wire.endTransmission();
}

void AnhTuanThongTin57_DS1307_Lib::datNgay(const char *ngayThangNam) {
  int ngay;
  int thang;
  int nam;
  sscanf(ngayThangNam, "%d/%d/%d", &thang, &ngay, &nam);

  _data[4] = chuyenDoiBCD(ngay);
  _data[5] &= 0x80;
  _data[5] = chuyenDoiBCD(thang);
  _data[6] |= chuyenDoiBCD(nam);

  Wire.beginTransmission(_address);
  Wire.write(0x04);
  Wire.write(_data[4]);
  Wire.write(_data[5]);
  Wire.write(_data[6]);
  Wire.endTransmission();
}

void AnhTuanThongTin57_DS1307_Lib::datNgayTrongTuan(uint8_t ngay) {
  _data[2] = chuyenDoiBCD(ngay);
}
