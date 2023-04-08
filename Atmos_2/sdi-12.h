#ifndef SDI_TWLEVE_h
#define SDI_TWLEVE_h

// The pin of the SDI-12 data bus
#define POWER_PIN -1  // The sensor power pin (or -1 if not switching power)
#define SENSOR_ADDRESS 1
#define AUTOMATIC false

#define DATA_PIN 11            // green off port1
#define SECONDARY_DATA_PIN 11  // stripe green off port1
#define THIRD_DATA_PIN 12      // blue
#define FOURTH_DATA_PIN 13     // stripe blue
#define PIN_SIZE 4

#include "Arduino.h"
#include <SDI12.h>
#include "sdi-read-event.h"
#include "log.h"
#include "serial-comms.h"

static String serialMsgStr = "0R0!";


class SDITwelve {
public:
  SDITwelve();
  // SDI12* eventBuffer();
  void sendByIndex(SDIReadEvent *event, size_t index);
  void setSDICommand(SDIReadEvent *event);
  void setSDICommandSimple(SDIReadEvent *event);
  void automaticSend(SDIReadEvent *event);
  bool hasFinalizedRequestContent(SDIReadEvent *event);
  void processSensorRequest(SDIReadEvent *event);
  void processActivity(SDIReadEvent *event);
  void begin();
  void end();
private:

  String getCommand(String request);
  bool pinIndexRead(size_t index);
  bool processInputSerial(SDIReadEvent *event, bool withClear);
  bool processInputSerial(SDIReadEvent *event, bool withClear, String preppend);
  String getCMDValues(SDIReadEvent *event);
  int getRequestIndex(String request);
  bool isReadyForAutoSend();
  const long mili = 1000;
  const long waitTime = 10;
  unsigned long lastTime = 0;
  bool send = false;
  size_t pinCount = 4;
  Logger logger;
  // pin definition array
  int dataPins[PIN_SIZE] = { DATA_PIN, SECONDARY_DATA_PIN, THIRD_DATA_PIN, FOURTH_DATA_PIN };
  SDI12 *mySDI12;
};

#endif