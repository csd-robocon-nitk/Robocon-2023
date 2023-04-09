#include <Servo.h>

Servo ESC;

int speed=0;

void setup() {    
  ESC.attach(9,1000,2000);
  //Calibration
  // ESC.write(180);
  // delay(5000);
  // ESC.write(0);
  // delay(5000);
  Serial.begin(115200);
  pinMode(2,INPUT_PULLUP);
  pinMode(3,INPUT_PULLUP);
  ESC.write(0);
}

void loop() {
  if(digitalRead(2)==LOW && speed < 175)
  {
      speed+=5;
      ESC.write(speed);
      delay(100);
  }

  if(digitalRead(3)==LOW && speed >5)
  { 
      speed-=5;
      ESC.write(speed);
      delay(100);
  }
  Serial.println(speed);
}