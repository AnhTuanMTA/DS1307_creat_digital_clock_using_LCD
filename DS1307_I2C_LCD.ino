/*
  Thư viện cho DS1307 RTC
  Tác giả: Anh Tuan Thong Tin 57
  Ngày: 28/08/2024
  Cập nhật: 25/11
*/


#include <AnhTuanThongTin57_DS1307_Lib.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <AnhTuanThongTin57_DS1307_Lib.h>

// Định nghĩa nút bấm
#define BUTTON_SELECT 2
#define BUTTON_UP 3
#define BUTTON_DOWN 4
#define BUTTON_EXIT 6

// Khởi tạo RTC và LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);
AnhTuanThongTin57_DS1307_Lib rtc(0x68);

// Biến trạng thái
int hour = 12, minute = 30, second = 0;
int alarmHour = 6, alarmMinute = 0;
bool editMode = false, alarmEnabled = false, settingAlarm = false;
int selectedField = 0;
unsigned long buttonDownTime = 0, blinkTime = 0, menuDisplayTime = 0;
bool buttonDownState = false, blinkState = false;

void setup() {
  pinMode(BUTTON_SELECT, INPUT_PULLUP);
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_EXIT, INPUT_PULLUP);
  pinMode(5, OUTPUT);

  lcd.init();
  lcd.backlight();
  rtc.begin();

  lcd.setCursor(0, 0);
  lcd.print("Nguyen Anh Tuan   ");
  delay(1000);
  lcd.setCursor(0, 0);
  lcd.print("Do An Mon Hoc     ");
  lcd.setCursor(0, 1);
  lcd.print("De_12             ");
  delay(1000);

  // Lấy thời gian từ RTC
  if (rtc.layThoiGian()) {  
    hour = rtc.layGio();      
    minute = rtc.layPhut();   
    second = rtc.layGiay();   
  }
}

void loop() {
  KiemtraMode();
  KiemtraExit();

  if (editMode) {
    KiemtraEditMode();
  } else {
    HienThiTG();
    checkAlarm();
    delay(100);
  }
}

void KiemtraMode() {
  if (digitalRead(BUTTON_SELECT) == LOW) {
    if (!buttonDownState) {
      buttonDownState = true;
      buttonDownTime = millis();
    }
    if (millis() - buttonDownTime >= 1000) {
      HienThiMenu();
      buttonDownState = false;
    }
  } else {
    buttonDownState = false;
  }
}

void KiemtraExit() {
  if (digitalRead(BUTTON_EXIT) == LOW) {
    if (editMode) toggleEditMode();
    else if (alarmEnabled) toggleAlarm();
    delay(200);
  }
}

void KiemtraEditMode() {
  updateBlink();
  HienThiEditMode();

  if (digitalRead(BUTTON_UP) == LOW) {
    incrementField();
    delay(200);
  }
  if (digitalRead(BUTTON_DOWN) == LOW) {
    decrementField();
    delay(200);
  }
  if (digitalRead(BUTTON_SELECT) == LOW) {
    changeSelectedField();
    delay(200);
  }
}

void HienThiMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1. ChinhThoiGian ");
  lcd.setCursor(0, 1);
  lcd.print("2. DatBaoThuc    ");
  menuDisplayTime = millis();
  while (millis() - menuDisplayTime < 5000) {
    if (digitalRead(BUTTON_UP) == LOW) {
      settingAlarm = false;
      toggleEditMode();
      break;
    }
    if (digitalRead(BUTTON_DOWN) == LOW) {
      settingAlarm = true;
      toggleEditMode();
      break;
    }
    delay(200);
  }
}

void updateBlink() {
  if (millis() - blinkTime >= 200) {
    blinkTime = millis();
    blinkState = !blinkState;
  }
}

void HienThiEditMode() {
  lcd.setCursor(0, 0);
  lcd.print(settingAlarm ? "DatBaoThuc       " : "ChinhThoiGian   ");
  lcd.setCursor(0, 1);
  lcd.print("Time:   ");

  lcd.print(blinkState && selectedField == 0 ? "**" : twoDigit(settingAlarm ? alarmHour : hour));
  lcd.print(":");
  lcd.print(blinkState && selectedField == 1 ? "**" : twoDigit(settingAlarm ? alarmMinute : minute));

  if (!settingAlarm) {
    lcd.print(":");
    lcd.print(blinkState && selectedField == 2 ? "**" : twoDigit(second));
  }
}

void HienThiTG() {
  if (rtc.layThoiGian()) {  
    hour = rtc.layGio();      
    minute = rtc.layPhut();  
    second = rtc.layGiay();   
  }

  lcd.setCursor(0, 0);
  lcd.print("Time:   ");
  lcd.print(twoDigit(hour));
  lcd.print(":");
  lcd.print(twoDigit(minute));
  lcd.print(":");
  lcd.print(twoDigit(second));

  lcd.setCursor(0, 1);
  char buffer[21]; 

  if (alarmEnabled) {
      snprintf(buffer, sizeof(buffer), "Bao thuc: %02d:%02d ", alarmHour, alarmMinute);
      lcd.print(buffer); 
  } else {
      lcd.print("Bao thuc: Off    "); // Hiển thị khi báo thức tắt
  }

}

void incrementField() {
  if (selectedField == 0) (settingAlarm ? alarmHour : hour) = ((settingAlarm ? alarmHour : hour) + 1) % 24;
  if (selectedField == 1) (settingAlarm ? alarmMinute : minute) = ((settingAlarm ? alarmMinute : minute) + 1) % 60;
  if (selectedField == 2 && !settingAlarm) second = (second + 1) % 60;
}

void decrementField() {
  if (selectedField == 0) (settingAlarm ? alarmHour : hour) = ((settingAlarm ? alarmHour : hour) + 23) % 24;
  if (selectedField == 1) (settingAlarm ? alarmMinute : minute) = ((settingAlarm ? alarmMinute : minute) + 59) % 60;
  if (selectedField == 2 && !settingAlarm) second = (second + 59) % 60;
}

void toggleEditMode() {
  // Chuyển chế độ chỉnh sửa
  editMode = !editMode;
  if (!editMode) {
    if (settingAlarm) {
      alarmEnabled = true;  
      lcd.setCursor(0, 1);
      lcd.print("Alarm Set       ");
      delay(1000);
    } else {
      char timeStr[9];
      snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", hour, minute, second);
      rtc.datThoiGian(timeStr); 
    }
  }
}

void changeSelectedField() {
  selectedField = (selectedField + 1) % (settingAlarm ? 2 : 3);
}

void toggleAlarm() {
  alarmEnabled = !alarmEnabled;
}

void checkAlarm() {
  if (alarmEnabled && hour == alarmHour && minute == alarmMinute) {
      digitalWrite(5, HIGH);
      delay(100);
      digitalWrite(5, LOW);
      delay(100);
  }
}

String twoDigit(int number) {
  return number < 10 ? "0" + String(number) : String(number);
}
