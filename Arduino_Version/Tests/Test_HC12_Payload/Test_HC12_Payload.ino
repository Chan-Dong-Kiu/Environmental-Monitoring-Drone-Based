#include <HardwareSerial.h>

#define HC12_TX_PIN 32
#define HC12_RX_PIN 33

HardwareSerial HC12Serial(2);

void setup() {
  Serial.begin(115200);
  
  // Initialize HC-12 on HardwareSerial 2
  HC12Serial.begin(9600, SERIAL_8N1, HC12_RX_PIN, HC12_TX_PIN);
  
  Serial.println("--- HC-12 Payload Test ---");
  Serial.println("Type something here and press Enter. It will be sent via Radio.");
  Serial.println("Any data received from Radio will be printed here.");
}

void loop() {
  // Read from computer (USB) and send to HC-12 (Radio)
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    Serial.print("Sending: ");
    Serial.println(msg);
    HC12Serial.println(msg);
  }

  // Read from HC-12 (Radio) and print to computer (USB)
  if (HC12Serial.available()) {
    String msg = HC12Serial.readStringUntil('\n');
    Serial.print("Received from GCS: ");
    Serial.println(msg);
  }
}
