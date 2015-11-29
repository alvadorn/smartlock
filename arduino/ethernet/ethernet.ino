#include <SPI.h>
#include <Ethernet.h>
#include <EthernetClient.h>

char server[] = "10.10.10.99";
byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xFE, 0x02 };

String bufferRead = "";
String bufferEther= "";
EthernetClient client;

void setup() {
  Serial.begin(115200);
  Ethernet.begin(mac);
  delay(100);
  Serial.print(Ethernet.localIP());
}

void loop() {
  if (Serial.available()) {
    while(Serial.available()) {
      bufferRead += Serial.read();
    }
    delay(100);
    if (client.connect(server, 8080)) {
      char s[1024];
      sprintf(s, "POST %s HTTP/1.1\r\nUser-Agent: curl/7.38.0\r\nHost: %s\r\nContent-type: %s\r\nContent-Length: %d\r\n\r\n%s", "/auth", server, "application/json", bufferRead.length(), bufferRead.c_str());
      client.print(s);
      bufferRead = "";
    }
  }
  if (client.available()) {
    while(client.available()) {
      bufferEther += client.read();
    }
    Serial.println(bufferEther.c_str());
  }
  if (!client.connected()) {
    Serial.println("discconted");
    client.stop();
  }
}
