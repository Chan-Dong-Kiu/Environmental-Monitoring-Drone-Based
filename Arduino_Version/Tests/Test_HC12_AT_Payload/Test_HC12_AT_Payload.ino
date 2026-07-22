#include <HardwareSerial.h>

#define HC12_TX_PIN 32
#define HC12_RX_PIN 33

HardwareSerial HC12Serial(2);

void setup() {
  Serial.begin(9600); // 9600 for AT commands
  HC12Serial.begin(9600, SERIAL_8N1, HC12_RX_PIN, HC12_TX_PIN);

  Serial.println("=== Payload HC-12 AT COMMAND MODE ===");
  Serial.println("1. Connect SET pin of HC-12 to GND.");
  Serial.println("2. Set Serial Monitor Baud Rate to 9600.");
  Serial.println("3. Type AT+DEFAULT and press Enter.");
}

void loop() {
  while (Serial.available()) {
    HC12Serial.write(Serial.read());
  }
  while (HC12Serial.available()) {
    Serial.write(HC12Serial.read());
  }
}
