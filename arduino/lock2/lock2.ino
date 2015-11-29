#include <LiquidCrystal.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <Keypad.h>
#include <Servo.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <Thread.h>
#include <ThreadController.h>
#include <TimerOne.h>

// Macros
#define KPAD_ROWS 4
#define KPAD_COLS 4
#define LED 13
#define MAX_LEN 16
#define RXBT A8
#define TXBT A9
#define CHANGE_LCD changeLcd = true;
#define LEDON digitalWrite(LED, HIGH);
#define LEDOFF digitalWrite(LED, LOW);
#define PORT 8080
#define URL "10.10.10.99"
#define JSONCONTENT "application/json"
#define LCD_MOVES 7



// Threads
ThreadController controller = ThreadController();
Thread closeDoor = Thread();
Thread moveRight = Thread();
Thread moveLeft = Thread();

// Global Variables
volatile byte screenCounter = LCD_MOVES;
int lastState = -1;
byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xFE, 0x02 };
const char keys[KPAD_ROWS][KPAD_COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
const byte rowPins[] = { 39, 37, 35, 33 };
const byte colPins[] = { 38, 36, 34, 32 };
char content[] = "application/json";
String name = "";
IPAddress ip(10, 10, 10, 50);

/*
 *  Initializing LCD
 * Pins configuration:
 * RS: 49
 * Enable: 48
 * D4: 47
 * D5: 46
 * D6: 45
 * D7: 44
 *
 */
LiquidCrystal lcd(49, 48, 47, 46, 45, 44);
String lcd_lines[2] = { "Digite matricula", "ou senha para entrar" };
String account = "";
String password = "";
byte state = 0;
boolean changeLcd = true;
boolean haveDelay = false;
long time = 0;
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

// Functions
void controllerStart() {
  controller.run();
}

void ledOff() {
  closeDoor.enabled = false;
  delay(8000);
  digitalWrite(LED, LOW);
  cancelKey();
}

void moveRightScreen() {
  lcd.scrollDisplayRight();
  screenCounter++;
  if (screenCounter == LCD_MOVES * 2) {
    moveLeft.enabled = true;
    moveRight.enabled = false;
  }

}

void moveLeftScreen() {
  lcd.scrollDisplayLeft();
  screenCounter--;
  if (screenCounter == 0) {
    moveLeft.enabled = false;
    moveRight.enabled = true;
  }
}

void refreshLcd() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(lcd_lines[0].c_str());
  lcd.setCursor(0, 1);
  lcd.print(lcd_lines[1].c_str());
  if (lastState == 4 || lastState == 5) {
    lcd.setCursor(14, 0);
    lcd.write(byte(0));
    lcd.write(byte(0));
    lcd.setCursor(14, 1);
    if (lastState == 4) {
      lcd.write(byte(1));
      lcd.write(byte(2));
      lastState = -1;
    } else {
      lcd.write(byte(3));
      lcd.write(byte(4));
    }
  }
  changeLcd = false;
}

bool isReservedKey(char key) {
  switch(key) {
    case 'A':
    case 'B':
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

void cancelKey() {
  state = 0;
  account = password = "";
  lcd_lines[0] = "  Digite matricula";
  lcd_lines[1] = "ou senha para entrar";
  CHANGE_LCD;
}

void verifyKey(char key) {
  if (key != NO_KEY) {

    if (state == 0) {

      if (isReservedKey(key)) {
        if (key == 'D') {
          state = 10;
          lcd_lines[0] = "Envie as informacoes";
          lcd_lines[1] = "   via bluetooth";
        } else {
          state = 1;
          lcd_lines[0] = "Digite a matricula:";
          lcd_lines[1] = "";
        }

        CHANGE_LCD
      }

    } else if (state == 1) {
      if (key == 'C') {
        cancelKey();
      } else if (!isReservedKey(key)) {
        if (account.length() < MAX_LEN) {
          account += key;
          lcd_lines[1] += "*";
          CHANGE_LCD
        }
      } else {
        state = 2;
        lcd_lines[0] = "Digite a senha:";
        lcd_lines[1] = "";
        CHANGE_LCD
      }

    } else if (state == 2) {
      if (key == 'C') {
        cancelKey();
      } else if (!isReservedKey(key)) {
        if (password.length() < MAX_LEN) {
          password += key;
          lcd_lines[1] += "*";
          CHANGE_LCD
        }
      } else {
        state = 3;
        lcd_lines[0] = "Autenticando...";
        lcd_lines[1] = "";
        CHANGE_LCD
        haveDelay = true;
        time = 2000;
      }
    } else if (state == 10) {
      if (key == 'C') {
        cancelKey();
      }
    }

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


void httpSkipHeader(String &buffer) {
  int index = buffer.indexOf("{");
  if (index != -1) {
    buffer.remove(0, index);
  } else {
    buffer = "";
  }
  index = buffer.indexOf("}");
  if (index != -1 || index != buffer.length() - 1) {
    buffer.remove(index + 1, buffer.length() -1);
  }
}

bool authenticated(String &buffer) {
  httpSkipHeader(buffer);
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(buffer.c_str());
  const char *status = root["authentication"];
  if (strcmp(status, "authorized") == 0) {
    const char *p_name = root["name"];
    name = p_name;
    return true;
  } else {
    return false;
  }
}


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


// Libs
class WebSocket {
private:
public:

	void static begin(byte *mac) {
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
      char s[1024];
      sprintf(s, "POST %s HTTP/1.1\r\nUser-Agent: curl/7.38.0\r\nHost: %s\r\nContent-type: %s\r\nContent-Length: %d\r\n\r\n%s", path, url, content_type, strlen(data), data);
      
      web.print(s);
      Serial.println(s);
      Serial.println(data);
		} else {
      Serial.println("Not connected");
    }
	}

  void static receive(boolean& status, String& buffer) {
    if (web.available()) {
      while(web.available())
        buffer += (char) web.read();
      web.stop();
      status = true;
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
  WebSocket::begin(mac);

  bluetooth.begin(9600);
  pinMode(LED, OUTPUT);
  LEDOFF

  // Configure Threads
  closeDoor.onRun(ledOff);
  closeDoor.setInterval(10);
  closeDoor.enabled = false;

  moveRight.onRun(moveRightScreen);
  moveRight.setInterval(500);
  moveRight.enabled = false;

  moveLeft.onRun(moveLeftScreen);
  moveLeft.setInterval(500);


  controller.add(&closeDoor);
  controller.add(&moveRight);
  controller.add(&moveLeft);

  // Timer1 config
  Timer1.initialize(200000);
  Timer1.attachInterrupt(controllerStart);
  Timer1.start();

}

void loop() {
	//Serial.print("State: ");
	//Serial.println(state);
	if(changeLcd) {
		refreshLcd();
	}
	if (haveDelay) {
		delay(time);
		haveDelay = false;
	}
  if (lastState == 5) {
    cancelKey();
    lastState = -1;
  } else if (lastState == 7) {
    LEDOFF
    lastState = -1;
    cancelKey();
  }
	char key = keypad.getKey();
  verifyKey(key);
  if (state == 0) {
    String buffer = "";
    bool received = verifyBluetooth(buffer);
    if (received) {
      sendHttpFromBluetooth(buffer, createJsonToSend);
      state = 3;
      lcd_lines[0] = "Autenticando...";
      lcd_lines[1] = "";
      CHANGE_LCD
    }
  } else if (state == 3) {
    String data;
    createJsonToSend(data, (char *) account.c_str(), (char *) password.c_str(), false, "");
    WebSocket::sendPostRequest(URL, PORT, "/auth", JSONCONTENT,  (char *) data.c_str());
    String resp;
    boolean received = false;
    WebSocket::receive(received, resp);
    Serial.print(resp);
    state = 5;
    if (received) {
      httpSkipHeader(resp);
      if (authenticated(resp)) {
        state = 4;
      }
    }
  } else if (state == 4) {
    lcd_lines[0] = "  Autenticado";
    lcd_lines[1] = " com sucesso!";
    state = 7;
    CHANGE_LCD
    haveDelay = true;
    time = 4000;
    lastState = 4;
  } else if (state == 5) {
    lcd_lines[0] = "   Falha na";
    lcd_lines[1] = " autenticacao";
    haveDelay = true;
    time = 4000;
    CHANGE_LCD
    lastState = 5;
  } else if (state == 6) {

    state = 3;
  } else if (state == 7) {
    lcd_lines[0] = "  Seja bem vindo";
    lcd_lines[1] = "     " + name;
    CHANGE_LCD;
    LEDON
    haveDelay = true;
    time = 4000;
    state = 0;
    lastState = 7;
  } else if (state == 10) {
    
  }
}
