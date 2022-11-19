# Arduino Codes
## List of Libraries
To be included to run the programs.
##### PS2X_lib
Interfacing PS2 controller with Arduino. Uses SPI communication protocol. 
- Data -> MOSI
- Command -> MISO
- Vibration -> 7.2-9V
- Ground -> GND
- VCC -> 3.3V
- Attention -> SS
- Clock -> SCK

![Color Coding of wires from PS2 Controller](https://camo.githubusercontent.com/3d316cbf722c5ed21d62ffc25a96f0cd5c3337f1a747059da8b6a244e53a2e3d/68747470733a2f2f73746f72652e637572696f7573696e76656e746f722e636f6d2f77702d636f6e74656e742f75706c6f6164732f323031392f30392f776972696e672e6a7067 "Color Coding of wires from PS2 Contoller")

##### ESPAsyncWebServer
Provides an easy way to build web servers.

##### ESPAsyncTCP
Required for ESPAsyncWebServer library

### References:
- PS2 controller with Arduino: https://create.arduino.cc/projecthub/electropeak/how-to-interface-ps2-wireless-controller-w-arduino-a0a813
- Add NodeMCU to Arduino IDE: https://create.arduino.cc/projecthub/electropeak/getting-started-w-nodemcu-esp8266-on-arduino-ide-28184f
