#DEPRICATED
### Similie SDI Controller

This respository supports our SDI controller board. In addition, it supports logic for storing serial data to a local SD card for offline opperation. Since our primary logic board does not support the SDI interface, we use an [Adafruit Feather M0 Adalogger](https://www.adafruit.com/product/2796 ) as a 32u4 co-processor. This board supports the (Arduino SDI-12)[https://github.com/EnviroDIY/Arduino-SDI-12] library. Though other pins may function for the SDI-12 interface, we have tested the functionality on pins 10, 11, and 13. We use these microprocessors to support our [Meter Group](https://www.metergroup.com/) rangs of environmental sensors.
