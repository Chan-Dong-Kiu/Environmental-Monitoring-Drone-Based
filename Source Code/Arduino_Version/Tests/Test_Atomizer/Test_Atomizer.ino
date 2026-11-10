#define MOSFET_PIN 26

void setup() {
  Serial.begin(115200);
  Serial.println("Atomizer (MOSFET) test");

  pinMode(MOSFET_PIN, OUTPUT);
  digitalWrite(MOSFET_PIN, LOW); // Start with atomizer OFF
}

void loop() {
  Serial.println("Turning Atomizer ON...");
  digitalWrite(MOSFET_PIN, HIGH);
  delay(3000); // Wait 3 seconds
  
  Serial.println("Turning Atomizer OFF...");
  digitalWrite(MOSFET_PIN, LOW);
  delay(3000); // Wait 3 seconds
}
