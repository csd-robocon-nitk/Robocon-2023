# PCB designs for the Mega and Nano PCB for Robocon 2023

The elephant and the rabbit robot each uses two PCBs. The PCBs have been designed in Autodesk Eagle.

## Mega PCB

![Mega PCB](/Media/Mega_PCB.png)
*Mega PCB*

The first PCB contains the Arduino Mega (Also compatible with Arduino Giga, and Arduino Due). This PCB is connected to all the motors responsible for the navigation of the robots and IMU module. The PCB also has pin headers for ESP-07 for WiFi communication.

## Nano PCB

![](/Media/Nano_PCB.png)
*Nano PCB*

The second PCB contains the Arduino Nano (Also compatible with boards of same form factor as Arduino Nano). This PCB controls the rest of the robot, which includes the shooting and picking mechanism. 

The boards have been designed such that they can be used together or as seperate standalone modules.