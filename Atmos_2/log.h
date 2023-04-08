#ifndef Logger_h
#define Logger_h

#include "Arduino.h"
#define DEBUG_MODE true

class Logger {
public:
  template<class T>
  void logln(T param);
  void logln(const char *);
  void logln(String);
  void logln(bool);
  void logln(unsigned int);
  void logln(unsigned long);
  void logln(int);

  template<class T>
  void log(T param);
  void log(String);
  void log(String, bool);
  void log(char param);
  void log(const char *);
  void log(size_t param);
private:
  void logString(String logString, bool ln);
};

#endif