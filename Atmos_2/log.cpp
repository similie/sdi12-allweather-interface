#include "log.h"
/*
 * Logging templates that can be toggled on and off
 * for debugging
 * 
 */
void Logger::logString(String logString, bool ln) {
  if (DEBUG_MODE == false) {
    return;
  }

  if (ln) {
    Serial.println(logString);
  } else {
    Serial.print(logString);
  }
}


void Logger::logln(bool print) {
  logString(String(print), true);
}
void Logger::logln(unsigned int print) {
  logString(String(print), true);
}

void Logger::logln(unsigned long print) {
  logString(String(print), true);
}

void Logger::logln(int print) {
  logString(String(print), true);
}

void Logger::logln(const char* print) {
  logString(String(print), true);
}

void Logger::logln(String print) {
  logString(print, true);
}

void Logger::log(String print) {
  logString(print, false);
}

void Logger::log(String print, bool ln) {
  logString(print, ln);
}

template<class T>
void Logger::logln(T param) {
  logString(String(param), true);
}

template<class T>
void Logger::log(T param) {
  logString(String(param), false);
}

void Logger::log(char param) {
  logString(String(param), false);
}

void Logger::log(const char* param) {
  logString(String(param), false);
}

void Logger::log(size_t param) {
  logString(String(param), false);
}