#include <SDI12.h>
//
#define CONSOLE_BAUD 115200
#define SERIAL_BAUD 9600  // The baud rate for the output serial port
#define DATA_PIN 10
// The pin of the SDI-12 data bus
#define POWER_PIN -1       // The sensor power pin (or -1 if not switching power)
#define SENSOR_ADDRESS 1
#define AUTOMATIC false

// include the SD library:
#include <SPI.h>
#include <SD.h>

const int READ_BUF_SIZE = 116;
const long mili = 1000;
const long waitTime = 10;
boolean send = false;
unsigned long lastTime = 0;
// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
// MKRZero SD: SDCARD_SS_PIN
const int chipSelect = 4;
File myFile;
// Define the SDI-12 bus
SDI12 mySDI12(DATA_PIN);
void setup(){
  Serial.begin(CONSOLE_BAUD);
  Serial1.begin(SERIAL_BAUD);
  while(!Serial1);
  // Power the sensors;
  if(POWER_PIN > 0){
    Serial.println("Powering up sensors...");
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
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  delay(2000);
}



String ourReading;
bool readyRead = false;

void loop() {
  static String serialMsgStr = "0R0!";
  static boolean serialMsgReady = true;
  static String sdiMsgStr;
  static boolean sdiMsgReady = false;
  static boolean verbatim = false;
  static boolean feedback = true;

  readSerial();
  
  if (AUTOMATIC && send ) {
    send = false;
    lastTime = millis();
    Serial.println(serialMsgStr);
    mySDI12.sendCommand(serialMsgStr);
  } else if (readyRead)  {
    readyRead = false;
    Serial.println(ourReading);
    mySDI12.sendCommand(ourReading);
    ourReading = "";
    
  }


  
  send = (millis() - lastTime) > (waitTime * mili);
  int avail = mySDI12.available();
  if (avail < 0) { mySDI12.clearBuffer(); } // Buffer is full; clear
  else if (avail > 0) {
    for (int a=0; a<avail; a++) {
      char inByte2 = mySDI12.read();
      if (inByte2 == '\n' || inByte2 == '\0') {
        sdiMsgReady = true;
      }
      else if (inByte2 == '!') {
        sdiMsgStr += "!";
        sdiMsgReady = true;
      }
      else {
        sdiMsgStr += String(inByte2);
      }
    }
  }
  if (sdiMsgReady) {
    Serial.println(sdiMsgStr);
    Serial1.println(sdiMsgStr);
     myFile = SD.open("all-weather-backup.txt", FILE_WRITE);
     myFile.println(sdiMsgStr);
     // close the file:
     myFile.close();
    sdiMsgReady = false;
    sdiMsgStr = "";
  }
}

int readSerial()

{
  int avail = Serial1.available();
  for (int i = 0; i < avail; i++)
  {
    char inChar = Serial1.read();
    Serial.print(inChar);
    if (inChar == '\n' || inChar == '\0')
    {
      readyRead = true;
      return 1;
    }
    if (inChar != '\r')
    {
      ourReading += String(inChar);
    }
  }

  return 0;
}
