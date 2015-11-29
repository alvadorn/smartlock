#include <LiquidCrystal.h>

LiquidCrystal lcd(12,11,10,9,8,7);
int state = 0;
boolean changed = true;
int length = 0;
String lcd_lines[2] = { "Digite matricula", "ou senha para entrar" };

void setup() {
  Serial.begin(115200);
  lcd.begin(16,2);
  lcd.noBlink();
}

void loop() {
  Serial.print(changed);
  if (Serial.available()) {
    byte buffered = Serial.read();
    if (buffered != state)
      changed = true;
    state = buffered;
    if (state == 1 || state == 2) {
      
    }
  }

  if (changed) {
    changed = false;
    refreshLcd();
  }
}

inline void refreshLcd() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(lcd_lines[0].c_str());
  lcd.print(lcd_lines[1].c_str());
  if (state == 4 || state == 5) {
    lcd.setCursor(13, 0);
    lcd.write(byte(0));
    lcd.write(byte(0));
    lcd.setCursor(13, 1);
    if (state == 4) {
      lcd.write(byte(1));
      lcd.write(byte(2));
    } else {
      lcd.write(byte(3));
      lcd.write(byte(4));
    }
  }
}
