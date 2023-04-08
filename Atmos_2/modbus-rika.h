#include "ModbusMaster.h"

#ifndef ModbusRika_h
#define ModbusRika_h

#define POWER_PIN_DEF 5

#include "rika-device.h"

// static const u_int16_t INVALID_DEVICE = 0xFFFD;

class ModbusRika
{
public:
  ModbusRika();
  ModbusRika(RikaDevice[], size_t);
  u_int16_t callNode(size_t);
  void begin(int port);
  void end();
  String wrapStringifiedOutputs();
  String stringifyOutput(size_t);
private:
  RikaDevice * devices;
  ModbusMaster node;
  const int POWER_PIN = POWER_PIN_DEF;
  void setupPin();
  size_t deviceLength = 0;
  const u_int16_t START_REG = 0x0000;
  const u_int16_t END_REG = START_REG + 8;
  u_int16_t readValue[MAX_PARAM_SIZE] = {};
  // void setAllInvalid();
  void setReadValues(size_t);
  int deviceAddress(uint16_t, RikaDevice *);
};


#endif