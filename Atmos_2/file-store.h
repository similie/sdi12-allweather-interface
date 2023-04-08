#ifndef FileStore_h
#define FileStore_h

#include "Arduino.h"
#include <SPI.h>
#include <SD.h>
#include "sdi-read-event.h"
#include "log.h"
#include "serial-comms.h"

#define DUMPALL false

class FileStore {
  public: 
    FileStore(/*SDIReadEvent * */);
    FileStore(/*SDIReadEvent * ,*/ bool);
    void initializeTheSDCard();
    void checkSDCard();
    bool kilFile(const char *filename);
    int getPopSize(String ourReading);
    void push(SDIReadEvent *event);
    void pop(SDIReadEvent *event);
    unsigned long getLineCount(int count);
    unsigned long getPosition();
    bool setPosition(unsigned long position);
    String getPopIndex(size_t index);
  private:
    Logger logger;
    const unsigned long SD_CARD_INIT_CHECK = 100000;
    const char *POSITION_FILE = "position";
    const char *FILE_NAME = "reads.txt";
    const char *TEMP_FILE = "temp.txt";
    const char *LOG_FILE = "log.txt";
    bool hasFile = false;
    unsigned long sdInstantiator = 0;
    // Arduino Ethernet shield: pin 4
    const int chipSelect = 4;
    // File myFile;0
    // Define the SDI-12 bus
    size_t pinCount = 4;
    bool active = true;
    // SDIReadEvent * event;
};

#endif