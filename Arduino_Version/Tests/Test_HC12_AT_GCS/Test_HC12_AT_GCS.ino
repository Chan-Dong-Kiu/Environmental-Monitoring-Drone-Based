#include <HardwareSerial.h>

#define HC12_TX_PIN 17
#define HC12_RX_PIN 16

HardwareSerial HC12Serial(2);
unsigned long last_send = 0;

void setup() {
  Serial.begin(115200);
  HC12Serial.begin(9600, SERIAL_8N1, HC12_RX_PIN, HC12_TX_PIN);

  Serial.println("=== GCS HC-12 AUTO AT TESTER ===");
  Serial.println("1. Connect SET to GND");
  Serial.println("2. Swap TX/RX if you see no response!");
  Serial.println("Sending 'AT' every 2 seconds...");
}

void loop() {
  if (millis() - last_send > 2000) {
    last_send = millis();
    HC12Serial.print("AT");
    Serial.println("-> Sent: AT");
  }

  if (HC12Serial.available()) {
    Serial.print("<- Response: ");
    while (HC12Serial.available()) {
      Serial.write(HC12Serial.read());
    }
    Serial.println();
  }
}
