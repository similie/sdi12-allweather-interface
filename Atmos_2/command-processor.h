#ifndef COMMAND_PROCESSOR_h
#define COMMAND_PROCESSOR_h

#include "Arduino.h"

#include "air-quality.h"


#include "sdi-read-event.h"
#include "file-store.h"
#include "log.h"
#include "sdi-12.h"
#include "serial-comms.h"

class CommandProcessor {
  public: 
    CommandProcessor();
    void loop();
    void setup();
  private: 
    AirQuality air;
    FileStore fileStore;
    SDITwelve sdi;
    Logger logger;
    SDIReadEvent *event = new SDIReadEvent();
    void processSerialCommand();
    void runAirQuality();
};

#endif