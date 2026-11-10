#define TRIG_PIN 18
#define ECHO_PIN 19

volatile uint32_t echo_start = 0;
volatile uint32_t echo_duration = 0;
volatile bool echo_done = false;

// Interrupt Service Routine for Echo Pin
void IRAM_ATTR echo_isr() {
    if (digitalRead(ECHO_PIN) == HIGH) {
        echo_start = micros();
    } else {
        echo_duration = micros() - echo_start;
        echo_done = true;
    }
}

void setup() {
  Serial.begin(115200);
  Serial.println("HY-SRF05 test with Interrupts");

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // Attach interrupt to ECHO_PIN
  attachInterrupt(digitalPinToInterrupt(ECHO_PIN), echo_isr, CHANGE);
}

void loop() {
  // Trigger SRF05
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Wait for echo to complete
  delay(50); // wait 50ms for interrupt to finish
  
  if (echo_done) {
    float distance_cm = (echo_duration / 2.0f) * 0.0343f;
    echo_done = false; // Reset flag
    
    if (distance_cm > 500.0) {
      Serial.println("Out of range (>5m)");
    } else {
      Serial.print("Distance: ");
      Serial.print(distance_cm);
      Serial.println(" cm");
    }
  } else {
    Serial.println("No echo received! Check ECHO_PIN wire.");
  }
  
  delay(1000); // Read every 1 second
}
