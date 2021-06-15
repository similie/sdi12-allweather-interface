#include <SDI12.h>
//
#define RESTORATION_BUFFER_SIZE 100
#define CONSOLE_BAUD 115200
#define SERIAL_BAUD 9600  // The baud rate for the output serial port
#define DATA_PIN 10
// The pin of the SDI-12 data bus
#define POWER_PIN -1       // The sensor power pin (or -1 if not switching power)
#define SENSOR_ADDRESS 1
#define AUTOMATIC false
#define DUMPALL false
#define DEBUG_MODE false

// include the SD library:
#include <SPI.h>
#include <SD.h>

static String serialMsgStr = "0R4!";

const int READ_BUF_SIZE = 116;
const long mili = 1000  ;
const long waitTime = 10;
boolean send = false;
unsigned long lastTime = 0;
char RESTORATION_BUFFER[RESTORATION_BUFFER_SIZE];
// set up variables using the SD utility library functions:
//Sd2Card card;
//SdVolume volume;
//SdFile root;
class SDIReadEvent {
  private:  
    String read;
    bool ready = false;
    bool working = false;
    bool fileKeeper = false;
  public:
    SDIReadEvent();
    void setRead(String read);
    void build(char value);
    void setReady();
    bool isReady();
    void setFileKeeper(bool keeping);
    bool fileKeeping();
    String getRead();
    void setToWork();
    bool isWorking();
    void print(char c);
    // void set
    void clear();
};
 
SDIReadEvent::SDIReadEvent() {}

void SDIReadEvent::print(char c) {
  log(c);
}

void SDIReadEvent::setFileKeeper(bool keeping) {
  fileKeeper = keeping;
}

bool SDIReadEvent::fileKeeping() {
  return fileKeeper;
}

bool SDIReadEvent::isWorking() {
  return working;
}

void SDIReadEvent::setToWork() {
  working = true;
}

void SDIReadEvent::setRead(String read) {
  this->read = read;
}

void SDIReadEvent::build(char value) {
  read += String(value);
}

void SDIReadEvent::setReady() {
  ready = true;
}

bool SDIReadEvent::isReady() {
  return ready;
}

String SDIReadEvent::getRead() {
  return read;
}

void SDIReadEvent::clear() {
  read = "";
  ready = false;
  working = false;
}

// change this to match your SD shield or module;m
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
// MKRZero SD: SDCARD_SS_PIN
const int chipSelect = 4;
// File myFile;0
// Define the SDI-12 bus
SDI12 mySDI12(DATA_PIN);
void setup(){
  Serial.begin(CONSOLE_BAUD);
  Serial1.begin(SERIAL_BAUD);
  while(!Serial1);
  // Power the sensors;
  if(POWER_PIN > 0){
    logln("Powering up sensors...");
    pinMode(POWER_PIN, OUTPUT);
    digitalWrite(POWER_PIN, HIGH);
    delay(200);
  }
  // Initiate serial connection to SDI-12 bus
  mySDI12.begin();
  delay(500);
  mySDI12.forceListen();
  // Print help text (may wish to comment out if used for communicating to software)

  if (!SD.begin(chipSelect)) {
    logln("initialization failed!");
    return;
  }
  logln("initialization done.");
  delay(2000);
}

// String ourReading;
const char * POSITION_FILE = "position";
const char * FILE_NAME = "reads.txt";
const char * TEMP_FILE  = "temp.txt";
const char * LOG_FILE  = "log.txt";

bool kilFile (const char * filename) {
  return SD.remove(filename);
}

String getPopIndex(size_t index) {
  return String("pop ") + String(index) + String(" ");
}

bool setPosition(unsigned long position) {
    if (SD.exists(POSITION_FILE) && !kilFile(POSITION_FILE)) {
       return false;
    }
    File myFile = SD.open(POSITION_FILE, FILE_WRITE);
    myFile.println(String(position));
    myFile.flush();
    myFile.close();
    return true;
}

unsigned long getPosition() {
  if (!SD.exists(POSITION_FILE)) {
    return 0;
  }
  File myFile = SD.open(POSITION_FILE);
  char readChar;
  String value;
  while(myFile.available()) {
    readChar = myFile.read();
    value += String(readChar);
  }
  myFile.close();
  return strtoul(value.c_str(), NULL, 10);
} 

unsigned long getLineCount(int count) {
  
  unsigned long position = getPosition();
  bool exists = SD.exists(FILE_NAME);
  log("GETTING LINES +++++ ");
  logln(exists);
  if (!exists) {
    File myFile = SD.open(FILE_NAME, FILE_WRITE);
    myFile.close();
    return position;
  }
  
  File myFile = SD.open(FILE_NAME);
  char readChar;
  size_t i = 0;

  const unsigned long SIZE = myFile.size();
  
  log("I HAVE A FILE OF SIZE ");
  logln(myFile.size());
  // log(getPopIndex(i));

  myFile.seek(position);
  
  while (myFile.available() && (i <= count || DUMPALL)) {
   char readChar = myFile.read();
   log(readChar);
   Serial1.print(readChar);
   position = myFile.position();
   if (readChar == '\n') {
    // Serial1.print('\n');
    Serial1.print(getPopIndex(i));
    i++;
   }
  }
  
  Serial1.write('\0');
  
  myFile.flush();
  myFile.close();

  if (position < SIZE) {
     setPosition(position);
  } else {
    // now we beach our position file
    kilFile(FILE_NAME);
    kilFile(POSITION_FILE);
  }
  return position;
}

void pop(SDIReadEvent * event) {

 if (event->fileKeeping()) {
    return;
 }
 event->setToWork();
 event->setFileKeeper(true);
 String localRead = event->getRead();
 int count = getPopSize(localRead);
 log("GETTING MY POP ON %%%%%%%%%%%%%%%%% ");
 logln(count);
 unsigned long pointerIndex = getLineCount(count);
 event->setFileKeeper(false);
}


void push(SDIReadEvent * event) {
   if (event->isWorking() || event->fileKeeping()) {
      return;
   }
   event->setToWork();
   event->setFileKeeper(true);
   
   String ourReading = event->getRead();
   File myFile = SD.open(FILE_NAME, FILE_WRITE);
   if (myFile) {
    log(ourReading.substring(5));
    myFile.print(ourReading.substring(5)); 
    myFile.flush();
    myFile.close(); 
   } else {
     Serial.print("error opening ");
     Serial.println(FILE_NAME);
   }
   event->setFileKeeper(false);
}

//void logIt(SDIReadEvent * event) {
//   if (event->fileKeeping()) {
//     return;
//   }
//   String ourReading = event->getRead();
//   event->setFileKeeper(true);
//   File myFile = SD.open(LOG_FILE, FILE_WRITE);
//   if (myFile) {
//      myFile.println(ourReading);
//      myFile.flush();
//      myFile.close(); 
//   } else {
//    logln("Failed to open log file");
//   }
//   event->setFileKeeper(false);
//}



int getPopSize(String ourReading) {
  String substring = ourReading.substring(4);
  return atoi(substring.c_str());
}

void setSDICommand(SDIReadEvent * event) {
   if (event->isWorking()) {
      return;
   }
   event->setToWork();
   String cmd = event->getRead();
   log(cmd);
   mySDI12.sendCommand(cmd);   
}

void processSerialCommand(SDIReadEvent * event) {
   String localRead = event->getRead();
   
   if (localRead.startsWith("pop")) {
     pop(event);
   } else if (localRead.startsWith("push")) {   
     push(event);
   } else if (localRead.startsWith("log")) {
     // logIt(event);
   } else if (localRead) {
     setSDICommand(event);
   }
   event->clear(); 
}



void processActivity(SDIReadEvent * event) {
   send = (millis() - lastTime) > (waitTime * mili);
   
   if (!event->isReady() || event->isWorking()) {
      return;
   }

   if (AUTOMATIC && send) {
    lastTime = millis();
    String read = event->getRead();
    //mySDI12.sendCommand(read);
    setSDICommand(event);
    send = false;         
  } else if (!AUTOMATIC)  {
    processSerialCommand(event);
  }  
}

SDIReadEvent * event = new SDIReadEvent();
SDIReadEvent * sdiEvent = new SDIReadEvent();

void loop() {
  readSerial(event);
  processActivity(event);
  processInputSerial(sdiEvent);
}


void processInputSerial(SDIReadEvent * event) {
  int avail = mySDI12.available();

  if (avail < 0) { mySDI12.clearBuffer(); event->clear(); } // Buffer is full; clear
  else if (avail > 0) {    
    event->setToWork();
    for (int a=0; a<avail; a++) {
      char inByte2 = mySDI12.read();
      if (inByte2 == '\n' || inByte2 == '\0') {
        event->setReady();
      }
      else if (inByte2 == '!') {
        event->build('!');
        event->setReady();
      }
      else if (!event->isReady() && inByte2 != '\r') {
        event->build(inByte2);
      }
    }
  }

  String sdiMsgStr = event->getRead();
  
  if (event->isReady()) {
    if (sdiMsgStr != "") {
     logln(sdiMsgStr);
     Serial1.println(sdiMsgStr);
    }
    event->clear();
  }
}

bool readSerial(SDIReadEvent * event)

{
   
   if (event->isWorking()) {
    return 0;
   }    
   int avail = Serial1.available();
   for (int i = 0; i < avail && !event->isReady(); i++)
   {
    char inChar = Serial1.read();
    //  /log(inChar);
    if (inChar == '\n' || inChar == '\0' )
    {
       
       if (inChar == '\n') {
         event->build(inChar);
       }
       event->setReady();
       return 1;
    }
     if (inChar != '\r')
     {
      event->build(inChar);
    }
  }
  return 0;
}

template<class T>
void logln(T param) {
 if (DEBUG_MODE == false) {
     return;
  }
  String logString = String(param);
  Serial.println(logString);
}
template<class T>
void log(T param) {
  
  if (DEBUG_MODE == false) {
     return;
  }
  String logString = String(param);
  Serial.print(logString);
}
