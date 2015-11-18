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
const byte mac[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x0A };
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
  root["account"] = account;
  root["password"] = password;
  if (bluetooth) {
    root["bt"] = "true";
    root["token"] = token;
  } else {
    root["bt"] = "false";
  }
  root.printTo(str);
}

inline void refreshLcd() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(byte(0));
  lcd.write(byte(0));
  lcd.print(lcd_lines[0].c_str());
  lcd.setCursor(0, 1);
  lcd.write(byte(1));
  lcd.write(byte(2));
  lcd.print(lcd_lines[1].c_str());
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
        lcd_lines[0] = "Autenticando...";
        lcd_lines[1] = "Autenticando...";
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

// Libs
class WebSocket {
private:
public:

	void static begin(const byte *mac) {
		Serial.begin(9600);
		if (Ethernet.begin((byte* ) mac) == 0) {
			Serial.println(Ethernet.localIP());
		} else {
			Serial.println("Falha no DHCP");
		}
   Serial.println("Teste");
	}

	void static sendPostRequest(char *url, int port, char *path, char *content_type, char *data) {
		if (web.connect(url, port)) {
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
			web.println(data);
		}
	}

};

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.noBlink();
  lcd.createChar(0, smiley_eye);
  lcd.createChar(1, smiley_happy_mouth[0]);
  lcd.createChar(2, smiley_happy_mouth[1]);
  lcd.createChar(3, smiley_sad_mouth[0]);
  lcd.createChar(4, smiley_sad_mouth[1]);
  keypad.setHoldTime(300);
  //WebSocket::begin(mac);
  //servo.attach(SERVOs  bluetooth.begin(9600);
}

void loop() {
  refreshLcd();
  char key = keypad.getKey();
  verifyKey(key);
  if (state == 0) {
    String buffer = "";
    bool received = verifyBluetooth(buffer);
    if (received) {
      sendHttpFromBluetooth(buffer, createJsonToSend);
    }
  }
	char url[] = "10.10.0.69";
	char path[] = "/auth";
	char content[] = "application/json";
  char data[400];
  String str;
  createJsonToSend(str, "2012100574", "password", true, "ahudash");
  Serial.println(str.c_str());
  delay(1000);
	//WebSocket::sendPostRequest(url,8080,path , content,  data);
}
