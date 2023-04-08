#include "serial-comms.h"

SerialComms::SerialComms() {
}

void SerialComms::start() {
  if (hasStarted) {
    return;
  }
  Serial1.begin(SERIAL_BAUD);
  hasStarted = true;
}

void SerialComms::end() {
  if (!hasStarted) {
    return;
  }
  Serial1.end();
  hasStarted = false;
}


void SerialComms::flush() {
  start();
  Serial1.flush();
}


void SerialComms::println(String print) {
  start();
  Serial1.println(print);
  flush();
}

void SerialComms::print(String print) {
  start();
  Serial1.print(print);
  flush();
}

void SerialComms::print(char print) {
  start();
  Serial1.print(print);
  flush();
}

void SerialComms::write(char print) {
  start();
  Serial1.print(print);
  flush();
}

void SerialComms::sendPong()
{
  Logger logger;
  logger.logln("Ping Event Detected");
  SerialComms::println("pong " + String(PROCESSOR_TYPE));
}

/**
 * sendErrorIndex - Sends an error for a read on a specific index. SDI 12 supports pins 
 *                8, 9, 10, 11, 14, 15, 16
 * 
 * @param size_t index - the pin index that failed 
 * @return void
 */
void SerialComms::sendErrorIndex(size_t index)
{
  String error = "ERROR_" + String(index) + " An unknown serial error occurred";
  SerialComms::println(error);
}


/**
 * readSerial - reads bytes off the serial bus
 *                
 * @param SDIReadEvent * eventt - contains the event data
 * @return bool - has a given command finished sending
 */
bool SerialComms::readSerial(SDIReadEvent *event) {
  if (event->isWorking() || event->isReady()) {
    return false;
  }
  Logger logger;
  int avail = Serial1.available();
  for (int i = 0; i < avail; i++) {
    char inChar = Serial1.read();
    if (inChar > 127) {
     continue;
    }
    // logger.log(inChar);
    if (inChar == '\n' || inChar == '\0') {
      //log(inChar);
      if (inChar == '\n') {
        event->build(inChar);
      }
      event->setReady();
      return true;
    }
    if (inChar != '\r') {
      event->build(inChar);
    }
  }
  return false;
}