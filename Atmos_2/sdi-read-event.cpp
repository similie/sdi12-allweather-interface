#include "sdi-read-event.h"

SDIReadEvent::SDIReadEvent() {}

void SDIReadEvent::print(char c)
{
  log(c);
}

void SDIReadEvent::setFileKeeper(bool keeping)
{
  fileKeeper = keeping;
}

bool SDIReadEvent::fileKeeping()
{
  return fileKeeper;
}

bool SDIReadEvent::isWorking()
{
  return working;
}

void SDIReadEvent::setToWork()
{
  working = true;
}

void SDIReadEvent::setRead(String read)
{
  this->read = read;
}

void SDIReadEvent::build(char value)
{
  read += String(value);
}

void SDIReadEvent::setReady()
{
  ready = true;
}

bool SDIReadEvent::isReady()
{
  return ready;
}

String SDIReadEvent::getRead()
{
  return read;
}

void SDIReadEvent::clear()
{
  read = "";
  ready = false;
  working = false;
}

