// This script is used for testing A2212 BLDC motors. 

// The BLDC motors can be controlled using the Servo library.
#include <Servo.h>

Servo ESC;

int speed=0;

void setup() {    
  ESC.attach(9,1000,2000);

  // Everytime the ESC is powered on, it needs to be calibrated.
  // Calibration takes about 10 secs. Maximum value should be supplied for the first 5 seconds and then minimum value for the next 5 seconds.
  ESC.write(180);
  delay(5000);
  ESC.write(0);
  delay(5000);

  // Initally stop the motor
  ESC.write(0);
}

void loop() {
  // Gradually increasing speed of motor
  if(speed <= 180)
  {
      speed+=5;
      ESC.write(speed);
      delay(100);
  }
}