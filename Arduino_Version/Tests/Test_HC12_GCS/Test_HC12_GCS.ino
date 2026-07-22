#include <HardwareSerial.h>

#define HC12_TX_PIN 17
#define HC12_RX_PIN 16

HardwareSerial HC12Serial(2);

unsigned long lastSendTime = 0;

void setup() {
  Serial.begin(115200);
  
  // ESP32 RX = 16, TX = 17
  // Make sure HC-12 TX connects to ESP32 RX (16)
  // Make sure HC-12 RX connects to ESP32 TX (17)
  HC12Serial.begin(9600, SERIAL_8N1, HC12_RX_PIN, HC12_TX_PIN);
  HC12Serial.setTimeout(100); // 100ms timeout for reading strings
  
  Serial.println("=== GCS HC-12 HELLO TEST ===");
  Serial.println("Sending 'hello from GCS' every 2.0 seconds...");
}

void loop() {
  // GCS sends every 2000 ms
  if (millis() - lastSendTime >= 2000) {
    lastSendTime = millis();
    HC12Serial.println("hello from GCS");
    Serial.println("-> Sent: hello from GCS");
  }

  // Read incoming complete string
  if (HC12Serial.available()) {
    String msg = HC12Serial.readStringUntil('\n');
    msg.trim(); // Remove trailing \r or spaces
    if (msg.length() > 0) {
      Serial.print("<- Received: ");
      Serial.println(msg);
    }
  }
}
