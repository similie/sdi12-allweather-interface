#include "command-processor.h"
#define CONSOLE_BAUD 115200

CommandProcessor processor;

void setup() {
  Serial.begin(CONSOLE_BAUD);
  processor.setup();
}

void loop() {
  processor.loop();
}





// void setup() {
//   Serial.begin(CONSOLE_BAUD);
// }

// void sendSerial(String output) {
//   Serial1.begin(38400);
//   for (size_t i = 0; i < output.length(); i++) {
//     char outChar = output.charAt(i);
//     Serial.write(outChar);
//     Serial1.write(outChar);  
//   }
//   Serial1.write('\n');
//   Serial1.flush();
//   Serial.print("\n");
//   Serial1.end();
// }


// void loop() {
//   String output = air.getReading();
//   sendSerial(output);
//   delay(1000);
// }