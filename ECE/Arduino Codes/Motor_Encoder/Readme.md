# Motor Encoder

## Sources
https://www.electroniclinic.com/arduino-dc-motor-speed-control-with-encoder-arduino-dc-motor-encoder/
https://github.com/curiores/ArduinoTutorials/tree/main/SpeedControl

## Motor Encoder Test Code
This code prints raw data from the hall effect sensors to the serial monitor. 

## Motor Encoder Position Code
This code uses the data from the slightly offset hall effect sensors to detect the direction of rotation
and prints the number of ticks the shaft has rotated ( +ve count in one direction and -ve in the other).

## Speed Control Code
This code uses the Motor encoder to take an input target speed and uses PID to set the speed of the motor to the target value.
