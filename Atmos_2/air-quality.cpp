#include "air-quality.h"

AirQuality::AirQuality() {
  this->rika = new ModbusRika(devices , DEVICE_LENGTH);
}

void AirQuality::setup() {
  rika->begin(this->AIR_QUALITY_PORT);
}

void AirQuality::shutdown() {
  rika->end();
}

String AirQuality::getName() {
  return String("airquality");
}

String AirQuality::getReading() {
  this->setup();
  for (size_t i = 0; i < DEVICE_LENGTH; i++) {
    u_int16_t result = rika->callNode(i);
    if (result == INVALID_DEVICE) {
      Serial.println("INVALID DEVICE");
      continue;
    }
    rika->stringifyOutput(i);
  }
  this->shutdown();
  return rika->wrapStringifiedOutputs();
}
