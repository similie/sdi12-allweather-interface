#include "modbus-rika.h"


ModbusRika::ModbusRika() {

}


ModbusRika::ModbusRika(RikaDevice devices[], size_t deviceLength) {
  this->devices = devices;
  this->deviceLength = deviceLength;
}

void ModbusRika::begin(int port) {
  Serial1.begin(port);
  node.begin(0, Serial1);
}

void ModbusRika::end() {
  Serial1.end();
}

int ModbusRika::deviceAddress(uint16_t index, RikaDevice * device) {
  for (size_t i = 0; i < device->getLength(); i++) {
   u_int16_t address = device->getAirQualitAddress(i);
   if (address == index) {
     return address;
   }
  }
  return -1;
}

void ModbusRika::setReadValues(size_t index) {
  uint16_t i = this->START_REG;
  while (node.available())
  {
    uint16_t received = node.receive();
    int address = deviceAddress(i, &this->devices[index]);
    if (address == -1) {
      i++;
      continue;
    }
    ReadValues read = { address: (uint16_t) address, value: received};
    this->devices[index].setRead(read);
    i++;
  }
}

String ModbusRika::wrapStringifiedOutputs() {
  String output = "";
  for (size_t i = 0; i <  this->deviceLength; i++) {
     RikaDevice device = this->devices[i];
     String deviceOutput = device.outputString;
     if (deviceOutput == "") {
       continue;
     }
     output += (i != 0 ? "," : "") + deviceOutput;
   }
   return output;
}

String ModbusRika::stringifyOutput(size_t index) {
  if (index > this->deviceLength - 1) {
    return "";
  }
  // Serial.print("MY READ ");
  // Serial.print(index);
  // Serial.print(" ");
  // Serial.print(this->devices[index].getSlave());
  // Serial.print(" ");
  // Serial.print(this->devices[index].getLength());
  // Serial.print(" ");
  // Serial.println(this->devices[index].hasValidRead());
  if (!this->devices[index].hasValidRead()) {
     return ""; 
  }

  return this->devices[index].stringifyOutput();
}





u_int16_t ModbusRika::callNode(size_t index) {
  if (index > this->deviceLength - 1) {
    return INVALID_DEVICE;
  }

  // resetting the device
  this->devices[index].resetRead();
  node.setSlave(this->devices[index].getSlave());

  uint8_t result = node.readHoldingRegisters(this->START_REG, this->END_REG);
  if (result != node.ku8MBSuccess) {
    return INVALID_DEVICE;
  }

  setReadValues(index);
  return VALID_DEVICE;
}