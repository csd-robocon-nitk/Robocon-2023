#include<Servo.h>

#define gantry_right A2
#define gantry_left A5
#define shooter_back A3
#define shooter_front A4
#define pick_up A7
#define pick_down A6
#define solenoid_pin 6
#define latch_pin 5

Servo latch_servo;

int l_angle = 0;

void setup() {
  pinMode(gantry_right, INPUT_PULLUP);
  pinMode(gantry_left, INPUT_PULLUP);
  pinMode(shooter_back, INPUT_PULLUP);
  pinMode(shooter_front, INPUT_PULLUP);
  pinMode(solenoid_pin, OUTPUT);
  latch_servo.attach(latch_pin);
  latch_servo.write(l_angle);
}

void loop() {
  // put your main code here, to run repeatedly:
}

void reload_seq() {
  while (digitalRead(gantry_left) != 0) {
    //Move gantry left
  }
  while (analogRead(pick_up) != 0) {
    //Move picking plate up and turn on solenoid
    digitalWrite(solenoid_pin, HIGH);
  }
  while (digitalRead(gantry_right) != 0) {
    //Move gantry right
  }
  while (digitalRead(gantry_left) != 0) {
    //Move gantry left and switch of solenoid
    digitalWrite(solenoid_pin, LOW);
  }
}