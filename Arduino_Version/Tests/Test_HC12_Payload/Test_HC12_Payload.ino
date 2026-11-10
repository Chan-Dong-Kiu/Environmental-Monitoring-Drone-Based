#include <HardwareSerial.h>

#define HC12_TX_PIN 32
#define HC12_RX_PIN 33

HardwareSerial HC12Serial(2);

unsigned long lastSendTime = 0;

void setup() {
  Serial.begin(115200);
  
  // ESP32 RX = 33, TX = 32
  // Make sure HC-12 TX connects to ESP32 RX (33)
  // Make sure HC-12 RX connects to ESP32 TX (32)
  HC12Serial.begin(9600, SERIAL_8N1, HC12_RX_PIN, HC12_TX_PIN);
  HC12Serial.setTimeout(100); // 100ms timeout for reading strings
  
  Serial.println("=== PAYLOAD HC-12 HELLO TEST ===");
  Serial.println("Sending 'hello from Payload' every 2.7 seconds...");
}

void loop() {
  // Payload sends every 2700 ms to avoid RF collision with GCS (which sends every 2000 ms)
  if (millis() - lastSendTime >= 2700) {
    lastSendTime = millis();
    HC12Serial.println("hello from Payload");
    Serial.println("-> Sent: hello from Payload");
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
