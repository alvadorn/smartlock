// Include libraries
#include <LiquidCrystal.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <Keypad.h>
#include <Servo.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

// Macros
#define KPAD_ROWS 4
#define KPAD_COLS 4
#define SERVO 5
#define MAX_LEN 16
#define RXBT A8
#define TXBT A9

// Constant variables
byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xFE, 0x02 };
const char keys[KPAD_ROWS][KPAD_COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
const byte rowPins[] = { 39, 37, 35, 33 };
const byte colPins[] = { 38, 36, 34, 32 };
IPAddress ip(10, 10, 10, 50);

// Global Variables

/*
 *  Initializing LCD
 * Pins configuration:
 * RS: 53
 * Enable: 51
 * D4: 49
 * D5: 47
 * D6: 45
 * D7: 43
 *
 */
LiquidCrystal lcd(53, 51, 49, 47, 45, 43);
//char lcd_lines[2][30] = { "Digite matricula", "ou senha para entrar" };
String lcd_lines[2] = { "Digite matricula", "ou senha para entrar" };
// EthernetClient
EthernetClient web;
// Keypad
Keypad keypad(makeKeymap((byte **)keys), (byte*)rowPins, (byte*)colPins, KPAD_ROWS, KPAD_COLS);
/*
 *  Servo Motor
 *  Red wire: 5v
 *  Brown wire: GND
 *  Orange wire: Signal
 */
Servo servo;
byte state = 0;
byte account_len = 0;
byte password_len = 0;
char account[MAX_LEN + 1];
char password[MAX_LEN + 1];
byte smiley_eye[8] =   {
    B00000,
    B00000,
    B01110,
    B01110,
    B01110,
    B00000,
    B00000 };
byte smiley_happy_mouth[2][8] = {
  { B00000,
    B00000,
    B00000,
    B11000,
    B01100,
    B00011,
    B00000 },
  { B00000,
    B00000,
    B00000,
    B00011,
    B00110,
    B11000,
    B00000
  }
};
byte smiley_sad_mouth[2][8] = {
  { B00000,
    B00000,
    B00000,
    B00011,
    B01100,
    B11000,
    B00000 },
  { B00000,
    B00000,
    B00000,
    B11000,
    B00110,
    B00011,
    B00000
  }
};
/*
 * Bluetooth module HC-05
 * RX = A8 -> RX arduino to TX HC-05
 * TX = A9 -> TX arduino to RX HC-05
 */
SoftwareSerial bluetooth(RXBT, TXBT);


// Functions definitions
void createJsonToSend(String& str, char *account, char *password, boolean bluetooth, char *token) {
  StaticJsonBuffer<400> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  //JsonObject& authenticate = root.createNestedObject("authenticate");
  root["account"] = account;
  root["password"] = password;
  if (bluetooth) {
    root["bt"] = "true";
    root["token"] = token;
  } else {
    root["bt"] = "false";
  }
  //JsonObject& root = authenticate.createNestedObject("authenticate");
  root.printTo(str);
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

bool isReservedKey(char key) {
  switch(key) {
    case 'A':
    case 'B':
    case 'C':
    case 'D':
      return true;
    case '*':
      return true;
    case '#':
      return true;
    default:
      return false;
  }
}

void verifyKey(char key) {
  if (key != NO_KEY) {

    if (state == 0) {

      if (isReservedKey(key)) {
        state = 1;
        account_len = password_len = 0;
        lcd_lines[0] = "Digite a matricula";
        lcd_lines[1] = "";
      }

    } else if (state == 1) {
      if (!isReservedKey(key)) {
        if (account_len < MAX_LEN) {
          account[account_len] = key;
          lcd_lines[1] += "*";
          account_len++;
        }
      } else {
        state = 2;
        lcd_lines[0] = "Digite a senha:";
        lcd_lines[1] = "";
      }

    } else if (state == 2) {

      if (!isReservedKey(key)) {
        if (password_len < MAX_LEN) {
          password[password_len] = key;
          lcd_lines[1] += "*";
          password_len++;
        }
      } else {
        state = 3;
      }
    }

  }
}

void keypadListener(KeypadEvent key) {
  switch(keypad.getState()) {
    case RELEASED:
      verifyKey(key);
      break;
    default:
      break;
  }
}

bool verifyBluetooth(String& buffer) {
  if (bluetooth.available()) {
    while(bluetooth.available()) {
      buffer += bluetooth.read();
    }
    return true;
  }
  return false;
}

void sendHttpFromBluetooth(String &buffer, void (*encode)(String&, char *, char *, boolean, char *)) {
  StaticJsonBuffer<400> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(buffer.c_str());
  const char *account = root["account"];
  const char *password = root["password"];
  const char *token  = root["token"];
  buffer = "";
  encode(buffer, (char *) account, (char *) password, true, (char *) token);
}


void httpSkipReader(String &buffer) {
  int index = buffer.indexOf("{");
  if (index != -1) {
    buffer.remove(0, index + 1);
  } else {
    buffer = "";
  }
}

bool authenticated(String &buffer) {
  httpSkipReader(buffer);
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(buffer.c_str());
  const char *status = root["authentication"];
  if (strcmp(status, "authorized")) {
    return true;
  } else {
    return false;
  }
}

void fixServo() {
  for (int i = 180; i >= 0; i--) {
    servo.write(0);
    delay(100);
  }
  delay(1000);
  for (int i = 0; i <= 30; i++) {
    servo.write(i);
    delay(100);
  }
}


// Libs
class WebSocket {
private:
public:

	void static begin(byte *mac) {
		Serial.begin(9600);
		if (Ethernet.begin(mac) == 0) {
			Serial.println(Ethernet.localIP());
		} else {
			Serial.println("Falha no DHCP");
      Serial.println(Ethernet.localIP());
		}
   Serial.println("Teste");
	}



  void static sendPostRequest(char *url, int port, char *path, char *content_type, char *data) {
		if (web.connect(url, port)) {
      /*
			Serial.println("teste");
			char buffer[1024];
			sprintf(buffer, "POST %s HTTP/1.1", path);
			web.println(buffer);
			web.println("User-Agent: curl/7.38.0");
			if (port == 80) {
				sprintf(buffer, "Host: %s", url);
			} else {
				sprintf(buffer, "Host: %s:%d", url, port);
			}
			web.println(buffer);
			sprintf(buffer, "Content-Type: %s", content_type);
			web.println(buffer);
			sprintf(buffer, "Accept: %s", content_type);
			web.println(buffer);
			sprintf(buffer, "Content-Length: %d", strlen(data));
			web.println(buffer);
			//web.println("Connection: close");
			web.println(data);*/
      /*String s = "POST " + path;
      s += "POST ";
      s += path;
      s += " HTTP/1.1\r\nUser-Agent: Arduino\r\nHost: ";
      s += url;
      s += "\r\nContent-Type: ";
      s += content_type;
      /*s += "\r\nAccept: ";
      s += content_type;
      s += "\r\nContent-Length: ";
      s += strlen(s.c_str());
      s += "\r\nConnection: close\r\n\r\n";(char *)s.c_str()
      s += data;*/

      char s[1024];
      sprintf(s, "POST %s HTTP/1.1\r\nUser-Agent: curl/7.38.0\r\nHost: %s\r\nContent-type: %s\r\nContent-Length: %d\r\n\r\n%s", path, url, content_type, strlen(data), data);
      
      web.print(s);
      web.stop();
      Serial.println(s);
      Serial.println(data);
		} else {
      Serial.println("Not connected");
    }
	}

};



void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.noBlink();
  /*lcd.createChar(0, smiley_eye);
  lcd.createChar(1, smiley_happy_mouth[0]);
  lcd.createChar(2, smiley_happy_mouth[1]);
  lcd.createChar(3, smiley_sad_mouth[0]);
  lcd.createChar(4, smiley_sad_mouth[1]);*/
  keypad.setHoldTime(300);
  WebSocket::begin(mac);
  //servo.attach(SERVO);
  //bluetooth.begin(9600);
  //f/]ixServo();
}

void loop() {
  //refreshLcd();
  char key = keypad.getKey();
  verifyKey(key);
  if (state == 0) {
    String buffer = "";
    bool received = verifyBluetooth(buffer);
    if (received) {
      sendHttpFromBluetooth(buffer, createJsonToSend);
      state = 3;
      lcd_lines[0] = "Autenticando...";
      lcd_lines[1] = "Autenticando...";
    }
  } else if (state == 3) {
    String received;

    if (authenticated(received)) {
      state = 4;
    } else {
      state = 5;
    }
  } else if (state == 4) {
    lcd_lines[0] = "    Autenticado";
    lcd_lines[1] = "   com sucesso!";
  } else if (state == 5) {
    lcd_lines[0] = "   Falha na";
    lcd_lines[1] = " autenticação";
  } else if (state == 6) {

    state = 3;
  } else if (state == 7) {

  }
	char url[] = "10.10.10.100";
	char path[] = "/auth";
	char content[] = "application/json";
  char data[400];
  String str;
  createJsonToSend(str, "2012100574", "password", true, "ahudash");
  //Serial.println(str.c_str());
  delay(1000);
	WebSocket::sendPostRequest(url,8080,path , content,  (char *)str.c_str());
  lcd.set(0,0);
  lcd.print("IMPRIME ESSA BOSTA");
  delay(1000);
}
