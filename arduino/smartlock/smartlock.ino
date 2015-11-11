// Include libraries
#include <LiquidCrystal.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <Keypad.h>
#include <Servo.h>


// Macros
#define KPAD_ROWS 4
#define KPAD_COLS 4
#define SERVO 5


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


// Functions definitions

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
  WebSocket::begin(mac);
  //servo.attach(SERVO);
}

void loop() {
	char url[] = "10.10.0.69";
	char path[] = "/auth";
	char content[] = "application/json";
	char data[] = "{ \"account\": \"2012100999\", \"password\": \"pass\", \"bt\": \"false\" }";
	WebSocket::sendPostRequest(url,8080,path , content,  data);
}
