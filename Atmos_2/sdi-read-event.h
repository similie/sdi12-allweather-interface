#ifndef SDIReadEvent_h
#define SDIReadEvent_h

#include "Arduino.h"

class SDIReadEvent
{
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

#endif