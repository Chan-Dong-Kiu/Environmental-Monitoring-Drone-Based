#include <HardwareSerial.h>

#define HC12_TX_PIN 32
#define HC12_RX_PIN 33

HardwareSerial HC12Serial(2);

unsigned long lastSendTime = 0;

void setup() {
  Serial.begin(115200);

  // Initialize HC-12 on HardwareSerial 2
  HC12Serial.begin(9600, SERIAL_8N1, HC12_RX_PIN, HC12_TX_PIN);

  Serial.println("--- Payload HC-12 Test ---");
  Serial.println("Sending 'hello from Payload' every 2 seconds...");
}

void loop() {
  // Send "hello" every 2 seconds (2000 ms) using non-blocking timer
  if (millis() - lastSendTime >= 2000) {
    lastSendTime = millis();
    HC12Serial.println("hello from Payload");
    Serial.println("-> Sent: hello from Payload");
  }

  // Read any incoming bytes from HC-12 and print to Serial Monitor
  if (HC12Serial.available()) {
    Serial.print("<- Received: ");
    while (HC12Serial.available()) {
      Serial.write(HC12Serial.read());
    }
  }
}
