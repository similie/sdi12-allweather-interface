#ifndef AirQuality_h
#define AirQuality_h

#include "Arduino.h"
#include "modbus-rika.h"


class AirQuality
{
public:
  AirQuality();
  String getReading();
  String getName();
private:
  const int AIR_QUALITY_PORT = 9600;
  static const  size_t DEVICE_LENGTH = 3;
  u_int8_t DEVICE_2[5] = {TEMP, HUMIDITY, PRESSURE, PM2_5, PM10};
  u_int8_t DEVICE_4[2] = {O3, CH4};
  u_int8_t DEVICE_5[3] = {CO2, NO2, S02};
  RikaDevice devices[DEVICE_LENGTH] = {RikaDevice(2, DEVICE_2, 5), RikaDevice(4, DEVICE_4, 2), RikaDevice(5, DEVICE_5, 3)};
  ModbusRika *rika;
  void setup();
  void shutdown();
};


#endif