#ifndef RikaDevice_h
#define RikaDevice_h

#include "Arduino.h"

enum  {
  TEMP,
  HUMIDITY,
  PRESSURE,
  PM2_5,
  PM10,
  O3,
  CH4,
  CO2,
  NO2,
  S02,
};


struct ReadValues {
  u_int16_t address;
  u_int16_t value;
};

static const u_int16_t INVALID_DEVICE = 0xFFFF;
static const u_int16_t INVALID_READ = 0xFFFD;
static const u_int16_t INVALID_ADDRESS = 0xFFFE;
static const u_int16_t VALID_DEVICE = 0x0000;
static const size_t MAX_PARAM_SIZE = 10;

class RikaDevice
{
public:
  RikaDevice(int);
  RikaDevice(int, u_int8_t *, size_t);
  String airQualityNames[MAX_PARAM_SIZE] = {"temp", "hum", "pressure", "pm2_5", "pm10", "o3", "ch4", "co2", "no2", "so2"};
  u_int16_t airQualityAddresses[MAX_PARAM_SIZE] = {
    0x0000, // temp
    0x0001, // humidity
    0x0002, // pressure
    0x0003, // pm2.5
    0x0004, // pm10
    0x0000, // O3
    0x0001, // ch4
    0x0000, // C02
    0x0001, // NO2
    0x0002, // SO2
  };
  int getSlave();
  size_t getLength();
  u_int16_t getAirQualitAddress(size_t);
  void setRead(ReadValues);
  void resetRead();
  ReadValues getRead(u_int16_t);
  bool hasValidRead();
  String outputString = "";
  String stringifyOutput();
private:
  int slave = 0;
  bool hasRead = false;
  u_int8_t * airQualityParams;
  size_t paramLength = 0;
  size_t readIndex = 0;
  ReadValues readValue[MAX_PARAM_SIZE] = {INVALID_READ};
};


#endif