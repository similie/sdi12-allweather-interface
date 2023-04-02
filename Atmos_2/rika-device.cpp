#include "rika-device.h"


RikaDevice::RikaDevice(int slave) {
  this->slave = slave;
}

RikaDevice::RikaDevice(int slave, u_int8_t *airQualityParams, size_t paramLength) {
 this->slave = slave;
 this->airQualityParams = airQualityParams;
 this->paramLength = paramLength;
}

int RikaDevice::getSlave() {
 return this->slave;
}
 
size_t RikaDevice::getLength() {
  return this->paramLength;
}

ReadValues RikaDevice::getRead(u_int16_t address) {

  for (size_t i = 0; i < MAX_PARAM_SIZE; i++) {
    if (this->readValue[i].value == INVALID_READ) {
      continue;
    }
    if (this->readValue[i].address == address) {
      return this->readValue[i];
    }
  }
  // return the last element which should be invalid
  return this->readValue[MAX_PARAM_SIZE - 1];
}

void RikaDevice::resetRead() {
  this->readIndex = 0;
  this->hasRead = false;
  this->outputString = "";
  for (size_t i = 0; i < MAX_PARAM_SIZE; i++) {
    this->readValue[i] = {
      INVALID_ADDRESS, INVALID_READ
    };
  }
}

bool RikaDevice::hasValidRead() {
  return this->hasRead;
}

String RikaDevice::stringifyOutput() {
  this->outputString = "";
  for (size_t i = 0; i < getLength(); i++) {
    u_int16_t address = this->getAirQualitAddress(i);
    ReadValues read = getRead(address);
    if (read.value == INVALID_READ) {
       continue;
    }
    u_int8_t value = this->airQualityParams[i];
    String name = airQualityNames[value];
    this->outputString += (i != 0 ? "," : "") + name + "=" + String(read.value);
  }
  return this->outputString;
}

void RikaDevice::setRead(ReadValues read) {
  if (this->readIndex >= MAX_PARAM_SIZE - 1) {
    this->readIndex = 0;
  }
  this->hasRead = true;
  this->readValue[this->readIndex] = read;
  this->readIndex++;
}

u_int16_t RikaDevice::getAirQualitAddress(size_t index){
 if (index > getLength() - 1) {
   return INVALID_DEVICE;
 }
 u_int8_t value = this->airQualityParams[index];
 return this->airQualityAddresses[value];
}