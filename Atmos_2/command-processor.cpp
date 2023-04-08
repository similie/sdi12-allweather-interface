#include "command-processor.h"



CommandProcessor::CommandProcessor() {}


void CommandProcessor::runAirQuality() {
  SerialComms::end();
  String output = air.getReading();
  String send = air.getName() + String(" ") + output;
  logger.logln(send);
  SerialComms::start();
  delay(200);
  SerialComms::println(send);
  SerialComms::flush();
}
/**
 * processSensorRequest - wrapper function for sending the command to the sensor
 *                
 * @param SDIReadEvent * eventt - contains the event data
 * @return void
 */
void CommandProcessor::processSerialCommand()
{
 
  if (!event->isReady()) {
    return;
  }

  String localRead = event->getRead();
  logger.log(localRead);
  if (localRead.startsWith("pop"))
  {
    fileStore.pop(event);
  }
  else if (localRead.startsWith("push"))
  {
    fileStore.push(event);
  }
  else if (localRead.startsWith("log"))
  {
    // logIt(event);
  }
  else if (localRead.startsWith("request"))
  {
    sdi.processSensorRequest(event);
  }
  else if (localRead.startsWith("sdi"))
  {
    sdi.processSensorRequest(event);
  }
  else if (localRead.startsWith(air.getName()))
  {
     runAirQuality();
  }
  else if (localRead.startsWith("ping"))
  {
    SerialComms::sendPong();
  }
  else if (localRead.startsWith("0R"))
  {
    sdi.setSDICommandSimple(event);
  } 
  else if (localRead.startsWith("$")) {
    logger.log(localRead);
  } 
  else
  {
    logger.log("Unknown Command ");
    logger.logln(localRead);
  }
  event->clear();
}

void CommandProcessor::loop() {
  SerialComms::readSerial(this->event);
  processSerialCommand();
}

void CommandProcessor::setup() {
  // fileStore.initializeTheSDCard();
  SerialComms::start();
  sdi.begin();
  delay(2000);
  logger.logln("SETTUP READY");
}