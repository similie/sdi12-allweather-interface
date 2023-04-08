
#include "file-store.h"

FileStore::FileStore() {
  // this->event = event;
}


FileStore::FileStore(/*SDIReadEvent * event, */ bool active) {
  // this->event = event;
  this->active = active;
}

void FileStore::initializeTheSDCard() {
  if (hasFile || !this->active) {
    return;
  }

  if (!SD.begin(chipSelect)) {
    logger.logln("initialization failed!");
    hasFile = false;
  } else {
    logger.logln("SD Card Available");
    hasFile = true;
  }
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
void FileStore::checkSDCard() {
  if (hasFile || !this->active) {
    return;
  }

  if (sdInstantiator >= SD_CARD_INIT_CHECK) {
    logger.logln("Attempting to power up the sd card reader");
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
bool FileStore::kilFile(const char *filename) {
  return SD.remove(filename);
}



/**
 * getPopIndex - appends the popped data with with the pop identity
 * and it's corresponding index
 * 
 * @param size_t index - the index of the line being returned
 * @return String - the concatenated string that contains the index
 */
String FileStore::getPopIndex(size_t index) {
  return String("pop ") + String(index) + String(" ");
}

/**
 * setPosition - retains the file position index for later retrieval
 * 
 * @param size_t index - the index of the line being returned
 * @return bool - true when everything works out
 */
bool FileStore::setPosition(unsigned long position) {
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
unsigned long FileStore::getPosition() {
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
  while (myFile.available()) {
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
unsigned long FileStore::getLineCount(int count) {
  unsigned long position = getPosition();
  bool exists = SD.exists(FILE_NAME);
  logger.log("GETTING LINES +++++ ");
  logger.logln(exists);
  if (!exists) {
    File myFile = SD.open(FILE_NAME, FILE_WRITE);
    myFile.close();
    return position;
  }

  File myFile = SD.open(FILE_NAME);
  char readChar;
  size_t i = 0;

  const unsigned long SIZE = myFile.size();

  logger.log("I HAVE A FILE OF SIZE ");
  logger.logln(myFile.size());
  // Postion the file from the last know position
  myFile.seek(position);
  size_t tickIndex = 0;

  while (myFile.available() && (i < count || DUMPALL)) {
    if (tickIndex == 0) {
      String identity = getPopIndex(i);
      logger.log(identity);
      SerialComms::print(identity);
    }
    tickIndex++;

    char readChar = myFile.read();
    logger.log(readChar);
    SerialComms::print(readChar);
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
void FileStore::pop(SDIReadEvent *event) {

  if (!hasFile) {
    logger.logln("File Failed to initialize");
    return;
  }

  if (event->fileKeeping()) {
    return;
  }
  event->setToWork();
  event->setFileKeeper(true);
  String localRead = event->getRead();
  int count = getPopSize(localRead);
  logger.log("Popping  ");
  logger.logln(count);
  unsigned long pointerIndex = getLineCount(count);
  event->setFileKeeper(false);
}

/**
 * push - pushes data lines requested over serial
 * 
 * @param SDIReadEvent * event - the event containing the data being filled
 * @return void
 */
void FileStore::push(SDIReadEvent *event) {

  if (!hasFile) {
    logger.logln("File Failed to initialize");
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
    logger.log(save);
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
int FileStore::getPopSize(String ourReading) {
  String substring = ourReading.substring(4);
  return atoi(substring.c_str());
}
