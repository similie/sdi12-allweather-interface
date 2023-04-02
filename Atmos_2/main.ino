// #include "modbus-rika.h"
#include "air-quality.h"
#include "Arduino.h"

AirQuality air;

void setup() {
  Serial.begin(115200);
}

void sendSerial(String output) {
  Serial1.begin(38400);
  for (size_t i = 0; i < output.length(); i++) {
    char outChar = output.charAt(i);
    Serial.write(outChar);
    Serial1.write(outChar);  
  }
  Serial1.write('\n');
  Serial1.flush();
  Serial.print("\n");
  Serial1.end();
}


void loop() {
  String output = air.getReading();
  sendSerial(output);
  delay(1000);
}