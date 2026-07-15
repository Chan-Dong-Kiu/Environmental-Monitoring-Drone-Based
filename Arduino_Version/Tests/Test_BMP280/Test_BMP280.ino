#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22 // Changed to 22 to fix strapping pin conflict!

Adafruit_BMP280 bmp; // I2C

void setup() {
  Serial.begin(115200);
  Serial.println(F("BMP280 test"));
  
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  
  unsigned status;
  status = bmp.begin(0x76); // Standard I2C address for BMP280, can be 0x77
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or I2C address!"));
    while (1) delay(10);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

void loop() {
    Serial.print(F("Temperature = "));
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");

    Serial.print(F("Pressure = "));
    Serial.print(bmp.readPressure() / 100.0F); // hPa
    Serial.println(" hPa");

    Serial.print(F("Approx altitude = "));
    Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
    Serial.println(" m");

    Serial.println();
    delay(2000);
}
