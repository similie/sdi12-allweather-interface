// include the SD library:
#define CONSOLE_BAUD 115200
#define DEBUG_MODE true
#include <SPI.h>
#include <SD.h>


const char * FILE_NAME = "boomo.txt";
const char * POSITION_FILE = "position";

const int chipSelect = 4;

void setup() {
  // put your setup code here, to run once:
 Serial.begin(CONSOLE_BAUD);
 delay(6000);
 if (!SD.begin(chipSelect)) {
   logln("initialization failed!");
   return;
 }
 logln("initialization done.");
 delay(2000);
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

void loop() {
  // put your main code here, to run repeatedly:
  File myFile = SD.open(FILE_NAME);
  if (!myFile) {
     logln("FILE IS BEACHED");
  }
  // int local = 0;
  unsigned long position = getPosition();
//  log("START POSITION ");
//  logln(position);
  
  myFile.seek(position);
  char readChar = 0;
  while(myFile.available() && readChar != '\n') {
    readChar = myFile.read();
    log(readChar);
    position = myFile.position();
    //local++;
  }

  bool available = myFile.available();
  
  
  log("END POSITION ");
  log(position);
  log(" ");
  logln(myFile.size());
  // logln("");
  myFile.close();
  
  if (!available) {
      if (!setPosition(0)) {
        logln("GOT FUCKING BEACHED RESETTING");
      }
  } else if (!setPosition(position)) {
     logln("SETTING POSITION GOT BEACHED");
  }


//
//  myFile.close();
//  File writeFile = SD.open(FILE_NAME, FILE_WRITE);
//  writeFile.seek(0);
//  for (int i = 0; i < local; i++) {
//    writeFile.write('\0');
//  }
//  
  
 
  // writeFile.flush();
 
  
  delay(1000);
}


bool kilFile (const char * filename) {
  return SD.remove(filename);
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
