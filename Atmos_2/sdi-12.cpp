#include "sdi-12.h"

SDITwelve::SDITwelve() {
  this->mySDI12 = new SDI12(DATA_PIN);
}


void SDITwelve::begin() {
  mySDI12->begin();
  delay(500);
  mySDI12->forceListen();
}

void SDITwelve::end() {
  mySDI12->end();
}


// SDI12 * SDITwelve::eventBuffer() {
//   return this->mySDI12;
// }

/**
 * sendByIndex - Sends a command on a specific index. SDI 12 supports pins 
 *                8, 9, 10, 11, 14, 15, 16
 * 
 * @param SDIReadEvent * event - contains the event data
 * @param size_t index - the pin index with the command 
 * @return void
 */
void SDITwelve::sendByIndex(SDIReadEvent *event, size_t index)
{
  int pin = dataPins[index];
  // log("GOT THIS PIN ");logln(pin);
  mySDI12->setDataPin(pin);
  String cmd = getCMDValues(event);
  logger.log(index);
  logger.log(" ");
  logger.log(cmd);
  mySDI12->sendCommand(cmd);
  if (!pinIndexRead(index))
  {
    SerialComms::sendErrorIndex(index);
  }
}

/**
 * getCMDValues - pulls a given command string from an SDIReadEvent object
 * 
 * @param SDIReadEvent * event - contains the serial data
 * @return String - the command
 */
String SDITwelve::getCMDValues(SDIReadEvent *event)
{
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
bool SDITwelve::pinIndexRead(size_t index)
{
  size_t count = 1000;
  size_t iter = 0;
  SDIReadEvent *sdiEvent = new SDIReadEvent();
  // this will return this identity back over the serial bus
  String sendContext = "result_" + String(index) + " ";
  while (!sdiEvent->isReady() && iter < count)
  {
    processInputSerial(sdiEvent, false, sendContext);
    iter++;
    delay(1);
  }
  sdiEvent->clear();
  delete sdiEvent;
  return iter < count;
}


/**
 * processInputSerial - wrapper function to process the sdi12 serial bus
 *                
 * @param SDIReadEvent * eventt - contains the event data
 * @param bool withClear - if the function should clear the event contents onces sent
 * @return bool - the read is read to be processed
 */
bool SDITwelve::processInputSerial(SDIReadEvent *event, bool withClear)
{
  return processInputSerial(event, withClear, "");
}

/**
 * processInputSerial - primary function to process the sdi12 serial bus
 *                
 * @param SDIReadEvent * eventt - contains the event data
 * @param bool withClear - if the function should clear the event contents onces sent
 * @param String preppend - any string value that should proceed the wrapped string values
 * @return bool - the read is read to be processed
 */
bool SDITwelve::processInputSerial(SDIReadEvent *event, bool withClear, String preppend)
{
  bool ready = hasFinalizedRequestContent(event);
  if (ready)
  {
    String sdiMsgStr = event->getRead();
    if (sdiMsgStr != "")
    {
      String send = preppend + sdiMsgStr;
      logger.logln(send);
      // sends out paylad back over the serial bus
      SerialComms::println(send);
    }
    //
    if (withClear)
    {
      event->clear();
    }
  }
  return ready;
}

/**
 * setSDICommand - Sends a command to all registered pins 
 *                
 * @param SDIReadEvent * event - contains the event data
 * @return void
 */
void SDITwelve::setSDICommand(SDIReadEvent *event)
{
  if (event->isWorking())
  {
    return;
  }
  event->setToWork();
  for (size_t i = 0; i < pinCount; i++)
  {
    sendByIndex(event, i);
  }
}


/**
 * processSensorRequest - wrapper function for sending the command to the sensor
 *                
 * @param SDIReadEvent * eventt - contains the event data
 * @return void
 */
void SDITwelve::processSensorRequest(SDIReadEvent *event)
{
  String localRead = event->getRead();
  int index = getRequestIndex(localRead);
  if (index > pinCount - 1)
  {
    return SerialComms::sendErrorIndex(index);
  }
  event->setToWork();
  // event->clear();
  SDIReadEvent *eventLocal = new SDIReadEvent();
  String cmd = getCommand(localRead);
  eventLocal->setRead(cmd);
  // Serial.print(cmd);
  // Serial.print(" ");
  // Serial.println(index);
  sendByIndex(eventLocal, index);
  eventLocal->clear();
  delete eventLocal;
}

/**
 * getCommand - gets the command that is to be sent to the device request_1 0R0!
 *                
 * @param String request - contains string request from the serial bus, i.e 
 * @return String - the command
 */
String SDITwelve::getCommand(String request)
{
  int index = request.indexOf(" ");
  return request.substring(index + 1);
}

/**
 * getRequestIndex - gets the pin index from the request string
 *                
 * @param String request - contains string request from the serial bus
 * @return int - the index that has been pulled from the string
 */
int SDITwelve::getRequestIndex(String request)
{
  request.replace("request_", "");
  return atoi(request.c_str());
}


/**
 * setSDICommandSimple - Sends a command to the first registered pin
 *                
 * @param SDIReadEvent * event - contains the event data
 * @return void
 */
void SDITwelve::setSDICommandSimple(SDIReadEvent *event)
{
  if (event->isWorking())
  {
    return;
  }
  event->setToWork();
  mySDI12->setDataPin(dataPins[0]);
  String cmd = getCMDValues(event);
  mySDI12->sendCommand(cmd);
}


/**
 * automaticSend - if automatic mode is selected it will just process the data after an interval
 *                
 * @param SDIReadEvent * eventt - contains the event data
 * @return void
 */
void SDITwelve::automaticSend(SDIReadEvent *event)
{
  event->setRead(serialMsgStr);
  lastTime = millis();
  setSDICommand(event);
  event->clear();
  send = false;
}


/**
 * hasFinalizedRequestContent - reads the actual sdi12 serial bus
 *                
 * @param SDIReadEvent * eventt - contains the event data
 * @param SDI12 * readBuffer - the sdi12 object being read
 */
bool SDITwelve::hasFinalizedRequestContent(SDIReadEvent *event)
{
  int avail = mySDI12->available();
  if (avail < 0)
  {
    mySDI12->clearBuffer();
    event->clear();
    return false;
  }
  else if (avail > 0)
  {
    event->setToWork();
    for (int a = 0; a < avail; a++)
    {
      char inByte2 = mySDI12->read();
      if (inByte2 == '\n' || inByte2 == '\0')
      {
        event->setReady();
      }
      else if (inByte2 == '!')
      {
        event->build('!');
        event->setReady();
      }
      else if (!event->isReady() && inByte2 != '\r')
      {
        event->build(inByte2);
      }
    }
  }
  return event->isReady();
}

/**
 * isReadyForAutoSend - checks of auto send is enabled an the send time has elapsed
 *                
 * @return bool - ready for sending
 */
bool SDITwelve::isReadyForAutoSend()
{
  return AUTOMATIC && (millis() - lastTime) > (waitTime * mili);
}

/**
 * processActivity - wrapper function called on every loop to process a serial request
 *                
 * @param SDIReadEvent * eventt - contains the event data
 * @return void
 */
void SDITwelve::processActivity(SDIReadEvent *event)
{
  if (isReadyForAutoSend())
  {
    automaticSend(event);
  }
  else if (!AUTOMATIC && event->isReady())
  {
   //  processSerialCommand(event);
  }
}

