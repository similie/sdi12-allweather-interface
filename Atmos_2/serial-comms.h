#ifndef SERIAL_COMMS_h
#define SERIAL_COMMS_h

#include "Arduino.h"
#include "sdi-read-event.h"
#include "log.h"
#define SERIAL_BAUD 76800
#define PROCESSOR_TYPE "MO_SAMD_21G18"

static bool hasStarted = false;

class SerialComms {
public:
  SerialComms();
  static void println(String);
  static void print(String);
  static void print(char);
  static void write(char);
  static void start();
  static void end();
  static void flush();
  static void sendPong();
  static void sendErrorIndex(size_t index);
  static bool readSerial(SDIReadEvent *event);

private:
  
  

};

#endif