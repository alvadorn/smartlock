// Include libraries
#include <LiquidCrystal.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <Keypad.h>


// Macros
#define KPAD_ROWS 4
#define KPAD_COLS 4



// Constant variables
const byte mac[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };
const char keys[KPAD_ROWS][KPAD_COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
const byte rowPins[] = { 39, 37, 35, 33 };
const byte colPins[] = { 38, 36, 34, 32 };

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

// Functions definitions


// Libs
class WebSocket {
private:
public:
	
	void static begin(const byte *mac) {
		Serial.begin(9600);
		if (Ethernet.begin((byte* ) mac) == 0) {
			Serial.println(Ethernet.localIP());
		}
	}

	void static sendPostRequest(char *url, char *path, char *content_type, char *data) {
		char buffer[1024];
		sprintf(buffer, "POST %s HTTP/1.1", path);
		web.println(buffer);
		sprintf(buffer, "Host: %s", url);
		web.println(buffer);
		web.println("User-Agent: Arduino");
		sprintf(buffer, "Content-Type: %s", content_type);
		web.println(buffer);
		sprintf(buffer, "Accept: %s", content_type);
		web.println(buffer);
		sprintf(buffer, "Content-length: %d", strlen(data));
		web.println(buffer);
		web.println("Connection: close");
		web.println(data);
	} 

};






void setup() {
  WebSocket::begin(mac);
  
}

void loop() {
	char url[] = "192.168.1.1";
	char path[] = "/contato";
	char content[] = "application/json";
	char data[] = "{ nome: \"testando\" }";
	WebSocket::sendPostRequest(url, path , content,  data);
}
