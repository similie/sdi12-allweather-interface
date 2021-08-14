#include <SDI12.h>

#define RESTORATION_BUFFER_SIZE 100
#define CONSOLE_BAUD 115200
#define SERIAL_BAUD 9600  // The baud rate for the output serial port
#define DATA_PIN 10 // green off port1
#define SECONDARY_DATA_PIN 11 // stripe green off port1
#define THIRD_DATA_PIN  13
// blue
// #define FOURTH_PIN 13 // stripe blue
// The pin of the SDI-12 data bus
#define POWER_PIN -1       // The sensor power pin (or -1 if not switching power)
#define SENSOR_ADDRESS 1
#define AUTOMATIC false
#define DUMPALL false
#define DEBUG_MODE true

// include the SD library:
#include <SPI.h>
#include <SD.h>

//static String serialMsgStr = "0R4!";
static String serialMsgStr = "0R0!";
// we don't want it checking on every loop
const unsigned long SD_CARD_INIT_CHECK = 100000;
const int READ_BUF_SIZE = 116;
const long mili = 1000  ;
const long waitTime = 10;
boolean send = false;
unsigned long lastTime = 0;
char RESTORATION_BUFFER[RESTORATION_BUFFER_SIZE];

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

// String ourReading;
const char * POSITION_FILE = "position";
const char * FILE_NAME = "reads.txt";
const char * TEMP_FILE  = "temp.txt";
const char * LOG_FILE  = "log.txt";
bool hasFile = false;
unsigned long sdInstantiator = 0;

SDIReadEvent * event = new SDIReadEvent();
SDIReadEvent * sdiEvent = new SDIReadEvent();
// Arduino Ethernet shield: pin 4 
const int chipSelect = 4;
// File myFile;0
// Define the SDI-12 bus
size_t pinCount = 3;
// pin definition array
int dataPins[] = {DATA_PIN, SECONDARY_DATA_PIN, THIRD_DATA_PIN};

SDI12 mySDI12(DATA_PIN);

/**
 * initializeTheSDCard  
 * 
 * We want the file to instantiate when we
 * have it in the device
 *
 * @return void
 */
void initializeTheSDCard()
{
  if (hasFile) {
     return;
  }

  if (!SD.begin(chipSelect)) {
    logln("initialization failed!");
    hasFile = false;
  } else {
    logln("SD Card Available");
    hasFile = true;
  }
}

/*
 * Initial Arduino setup function. All init actions go here
 */
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

  delay(1000);
  // Initiate serial connection to SDI-12 bus
  mySDI12.begin();
  delay(500);
  mySDI12.forceListen();
  // start the SD Card reader
  initializeTheSDCard();

  
  // pinMode(THIRD_DATA_PIN, INPUT_PULLDOWN);
  
  logln("Startup done.");
  //delay(2000);
}

/**
 * checkSDCard 
 * 
 * If we pop a new SD card after it has booted. We want to be 
 * able to respond
 * 
 * @param const * char filename - the file to remove
 * @return bool - true if the file is removed
 */
void checkSDCard()
{
  if (hasFile) {
     return;
  }

  if (sdInstantiator >= SD_CARD_INIT_CHECK) {
    logln("Attempting to power up the sd card reader");
    initializeTheSDCard();
    sdInstantiator = 0;
  } else {
    sdInstantiator++;
  }
  
}

/**
 * kilFile - removes a file from the sd card by filename
 * 
 * @param const * char filename - the file to remove
 * @return bool - true if the file is removed
 */
bool kilFile (const char * filename) {
  return SD.remove(filename);
}

/**
 * getPopIndex - appends the popped data with with the pop identity
 * and it's corresponding index
 * 
 * @param size_t index - the index of the line being returned
 * @return String - the concatenated string that contains the index
 */
String getPopIndex(size_t index) {
  return String("pop ") + String(index) + String(" ");
}

/**
 * setPosition - retains the file position index for later retrieval
 * 
 * @param size_t index - the index of the line being returned
 * @return bool - true when everything works out
 */
bool setPosition(unsigned long position) {
    if (SD.exists(POSITION_FILE) && !kilFile(POSITION_FILE)) {
       hasFile = false;
       return false;
    }
    File myFile = SD.open(POSITION_FILE, FILE_WRITE);
    myFile.println(String(position));
    myFile.flush();
    myFile.close();
    return true;
}

/**
 * getPosition - gets the value of the file position stored in file 
 * 
 * @return unsigned long - the value of the last saved position
 */
unsigned long getPosition() {
  if (!SD.exists(POSITION_FILE)) {
    return 0;
  }
  File myFile = SD.open(POSITION_FILE);
  if (!myFile) {
      hasFile = false;
      return 0;
  }
  char readChar;
  String value;
  while(myFile.available()) {
    readChar = myFile.read();
    value += String(readChar);
  }
  myFile.close();
  return strtoul(value.c_str(), NULL, 10);
} 

/**
 * getLineCount - gets the number of lines in the storage file as input
 * 
 * @param int count - the number of lines being requested
 * @return unsigned long - the value of the last traversed position
 */
unsigned long getLineCount(int count) {
  unsigned long position = getPosition();
  bool exists = SD.exists(FILE_NAME);
  log("GETTING LINES +++++ ");logln(exists);
  if (!exists) {
    File myFile = SD.open(FILE_NAME, FILE_WRITE);
    myFile.close();
    return position;
  }
  
  File myFile = SD.open(FILE_NAME);  
  char readChar;
  size_t i = 0;

  const unsigned long SIZE = myFile.size();
  
  log("I HAVE A FILE OF SIZE ");logln(myFile.size());
  // Postion the file from the last know position
  myFile.seek(position);
  size_t tickIndex = 0; 

  while (myFile.available() && (i < count || DUMPALL)) {
   if (tickIndex == 0) {
    String identity = getPopIndex(i);
    log(identity);
    Serial1.print(identity);
   }
   tickIndex++;
   
   char readChar = myFile.read();
   log(readChar);
   Serial1.print(readChar);
   position = myFile.position();
   if (readChar == '\n') {
    i++;
    tickIndex = 0;
   }
  }

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

/**
 * pop - pops lines from the storage file requested over serial
 * 
 * @param SDIReadEvent * event - the event containing the data being filled
 * @return void
 */
void pop(SDIReadEvent * event) {

 if (!hasFile) {
     logln("File Failed to initialize");
     return;
 }

 if (event->fileKeeping()) {
    return;
 }
 event->setToWork();
 event->setFileKeeper(true);
 String localRead = event->getRead();
 int count = getPopSize(localRead);
 log("Popping  ");logln(count);
 unsigned long pointerIndex = getLineCount(count);
 event->setFileKeeper(false);
}

/**
 * push - pushes data lines requested over serial
 * 
 * @param SDIReadEvent * event - the event containing the data being filled
 * @return void
 */
void push(SDIReadEvent * event) {

   if (!hasFile) {
     logln("File Failed to initialize");
     return;
   }
  
   if (event->isWorking() || event->fileKeeping()) {
      return;
   }
   event->setToWork();
   event->setFileKeeper(true);
   
   String ourReading = event->getRead();
   File myFile = SD.open(FILE_NAME, FILE_WRITE);
   if (myFile) {
    String save = ourReading.startsWith("push") ? ourReading.substring(5) : ourReading;
    log(save);
    myFile.print(save); 
    myFile.flush();
    myFile.close(); 
   } else {
     hasFile = false;
     Serial.print("error opening ");
     Serial.println(FILE_NAME);
   }
   event->setFileKeeper(false);
}

/**
 * getPopSize - pulls the interger pop value from the request string
 * 
 * @param String ourReading - the string over the serial bus i.e. pop 5
 * @return int - the interger value to pop
 */
int getPopSize(String ourReading) {
  String substring = ourReading.substring(4);
  return atoi(substring.c_str());
}

/**
 * getCMDValues - pulls a given command string from an SDIReadEvent object
 * 
 * @param SDIReadEvent * event - contains the serial data
 * @return String - the command
 */
String getCMDValues(SDIReadEvent * event) {
   String cmd = event->getRead();
   return cmd;
}

/**
 * pinIndexRead - reads the data off a specfic pin index. SDI 12 supports pins 
 *                8, 9, 10, 11, 14, 15, 16
 * 
 * @param size_t index - the pin index to read 
 * @return bool - there was a valid command read processed
 */
bool pinIndexRead(size_t index) {
  size_t count = 1000;
  size_t iter = 0;
  // this will return this identity back over the serial bus
  String sendContext = "result_" + String(index) + " ";
  while(!sdiEvent->isReady() && iter < count) {
    readSerial(event);
    processInputSerial(sdiEvent, &mySDI12, false, sendContext);
    iter++;
    delay(1);
  } 
  sdiEvent->clear();
  return iter < count;
}

/**
 * sendErrorIndex - Sends an error for a read on a specific index. SDI 12 supports pins 
 *                8, 9, 10, 11, 14, 15, 16
 * 
 * @param size_t index - the pin index that failed 
 * @return void
 */
void sendErrorIndex(size_t index) {
  String error = "ERROR_" + String(index) + " An unknown serial error occurred";
  Serial1.println(error);
}

/**
 * sendByIndex - Sends a command on a specific index. SDI 12 supports pins 
 *                8, 9, 10, 11, 14, 15, 16
 * 
 * @param SDIReadEvent * event - contains the event data
 * @param size_t index - the pin index with the command 
 * @return void
 */
void sendByIndex(SDIReadEvent * event, size_t index) {
  int pin = dataPins[index];
  log("GOT THIS PIN ");logln(pin);
  mySDI12.setDataPin(pin);
  String cmd = getCMDValues(event);
  log(index);log(" ");log(cmd);
  mySDI12.sendCommand(cmd);    
  if (!pinIndexRead(index)) {
    sendErrorIndex(index);
  } 
}

/**
 * setSDICommand - Sends a command to all registered pins 
 *                
 * @param SDIReadEvent * event - contains the event data
 * @return void
 */
void setSDICommand(SDIReadEvent * event) {
   if (event->isWorking()) {
      return;
   }
   event->setToWork();
   for (size_t i = 0; i < pinCount; i++) {
    sendByIndex(event, i);
   }
}

/**
 * setSDICommandSimple - Sends a command to the first registered pin
 *                
 * @param SDIReadEvent * event - contains the event data
 * @return void
 */
void setSDICommandSimple(SDIReadEvent * event) {
   if (event->isWorking()) {
      return;
   }
   event->setToWork();
   mySDI12.setDataPin(dataPins[0]);
   String cmd = getCMDValues(event);
   // logln(cmd);
   mySDI12.sendCommand(cmd);
}

/**
 * getRequestIndex - gets the pin index from the request string
 *                
 * @param String request - contains string request from the serial bus
 * @return int - the index that has been pulled from the string
 */
int getRequestIndex(String request) {
  request.replace("request_", "");
  return atoi(request.c_str());
}

/**
 * getCommand - gets the command that is to be sent to the device request_1 0R0!
 *                
 * @param String request - contains string request from the serial bus, i.e 
 * @return String - the command
 */
String getCommand(String request) {
  int index = request.indexOf(" ");
  return request.substring(index + 1);
}

/**
 * processSensorRequest - wrapper function for sending the command to the sensor
 *                
 * @param SDIReadEvent * eventt - contains the event data
 * @return void
 */
void processSensorRequest(SDIReadEvent * event) {
  String localRead = event->getRead();
  int index = getRequestIndex(localRead);
  if (index > pinCount - 1) {
    return sendErrorIndex(index);
  }
  event->setToWork();
  String cmd = getCommand(localRead);
  event->setRead(cmd);
  sendByIndex(event, index);
  event->clear();
}

/**
 * sendPong
 * 
 * Used to respond to the the primary that's checking if the
 * serial device is available
 * 
 * @return void
 */
void sendPong()
{
  logln("Ping Event Detected");
  Serial1.println("pong");
}

/**
 * processSerialCommand - sends the command to the correct function for processing
 *                
 * @param SDIReadEvent * eventt - contains the event data
 * @return void
 */
void processSerialCommand(SDIReadEvent * event) {
   String localRead = event->getRead();
   if (localRead.startsWith("pop")) {
     pop(event);
   } else if (localRead.startsWith("push")) {   
     push(event);
   } else if (localRead.startsWith("log")) {
     // logIt(event);
   } else if (localRead.startsWith("request")) {
     processSensorRequest(event);
   } else if (localRead.startsWith("ping")) {
     sendPong();
   } else if (localRead.startsWith("0R")) {
     setSDICommandSimple(event);
   } else {
     log("Unknown Command ");logln(localRead);
   }
   event->clear(); 
}

/**
 * automaticSend - if automatic mode is selected it will just process the data after an interval
 *                
 * @param SDIReadEvent * eventt - contains the event data
 * @return void
 */
void automaticSend(SDIReadEvent * event) {  
   event->setRead(serialMsgStr);
   lastTime = millis();  
   setSDICommand(event); 
   event->clear(); 
   send = false;       
}

/**
 * isReadyForAutoSend - checks of auto send is enabled an the send time has elapsed
 *                
 * @return bool - ready for sending
 */
bool isReadyForAutoSend() {
  return AUTOMATIC && (millis() - lastTime) > (waitTime * mili);
}

/**
 * processActivity - wrapper function called on every loop to process a serial request
 *                
 * @param SDIReadEvent * eventt - contains the event data
 * @return void
 */
void processActivity(SDIReadEvent * event) {     
   if (isReadyForAutoSend()) {
    automaticSend(event);
  } else if (!AUTOMATIC && event->isReady())  {
    processSerialCommand(event);
  }  
}

/**
 * loop - primary arduino loop
 *                
 * @return void
 */
void loop() {
  readSerial(event);
  processActivity(event);
  processInputSerial(sdiEvent, &mySDI12, true);
  checkSDCard();
}

/**
 * processInputSerial - wrapper function to process the sdi12 serial bus
 *                
 * @param SDIReadEvent * eventt - contains the event data
 * @param SDI12 * readBuffer - the sdi12 object being read
 * @param bool withClear - if the function should clear the event contents onces sent
 * @return bool - the read is read to be processed
 */
bool processInputSerial(SDIReadEvent * event, SDI12 * readBuffer, bool withClear) {
  return processInputSerial(event, readBuffer, withClear, "");
}

/**
 * processInputSerial - primary function to process the sdi12 serial bus
 *                
 * @param SDIReadEvent * eventt - contains the event data
 * @param SDI12 * readBuffer - the sdi12 object being read
 * @param bool withClear - if the function should clear the event contents onces sent
 * @param String preppend - any string value that should proceed the wrapped string values
 * @return bool - the read is read to be processed
 */
bool processInputSerial(SDIReadEvent * event, SDI12 * readBuffer, bool withClear, String preppend) {
  bool ready = hasFinalizedRequestContent(event, readBuffer);
  if (ready) {
    String sdiMsgStr = event->getRead();
    if (sdiMsgStr != "") {
     String send = preppend + sdiMsgStr;
     logln(send);
     // sends out paylad back over the serial bus
     Serial1.println(send);
    }
    // 
    if (withClear) {
      event->clear();  
    }
  }
  return ready;
}

/**
 * processInputSerial - reads the actual sdi12 serial bus
 *                
 * @param SDIReadEvent * eventt - contains the event data
 * @param SDI12 * readBuffer - the sdi12 object being read
 * @return bool - the read is read to be processed
 */
bool hasFinalizedRequestContent(SDIReadEvent * event, SDI12 * readBuffer) {
  int avail = readBuffer->available();
  if (avail < 0) { 
   readBuffer->clearBuffer(); 
   event->clear();    
   return false;
  } else if (avail > 0) {    
    event->setToWork();
    for (int a=0; a<avail; a++) {
      char inByte2 = readBuffer->read();
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
  return event->isReady();
}

/**
 * readSerial - reads bytes off the serial bus
 *                
 * @param SDIReadEvent * eventt - contains the event data
 * @return bool - has a given command finished sending
 */
bool readSerial(SDIReadEvent * event)
{
   if (event->isWorking() || event->isReady()) {
    return false;
   }    
   int avail = Serial1.available();
   for (int i = 0; i < avail; i++)
   {
    char inChar = Serial1.read();
    // log(inChar);
    
    if (inChar == '\n' || inChar == '\0' )
    {
      //log(inChar);
       if (inChar == '\n') {
         event->build(inChar);
       }
       event->setReady();
       return true;
     }
     if (inChar != '\r')
     {
      event->build(inChar);
     }
  }
  return false;
}

/*
 * Logging templates that can be toggled on and off
 * for debugging
 * 
 */
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
