#include <HardwareSerial.h>

#define HC12_TX_PIN 32
#define HC12_RX_PIN 33

HardwareSerial HC12Serial(2);

void setup() {
  Serial.begin(115200);

  // Initialize HC-12 on HardwareSerial 2
  HC12Serial.begin(9600, SERIAL_8N1, HC12_RX_PIN, HC12_TX_PIN);

  Serial.println("--- HC-12 Passthrough Mode ---");
  Serial.println("Cắm chân SET vào GND để vào chế độ AT.");
}

void loop() {
  // Đọc từng byte từ máy tính và đẩy thẳng vào HC-12
  while (Serial.available()) {
    HC12Serial.write(Serial.read());
  }

  // Đọc từng byte từ HC-12 và đẩy thẳng ra màn hình máy tính
  while (HC12Serial.available()) {
    Serial.write(HC12Serial.read());
  }
}
