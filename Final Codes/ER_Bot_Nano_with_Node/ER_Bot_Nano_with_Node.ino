#include <Servo.h>

#define LF 0x0A

// Defining pins for each component
#define gantry_right A2
#define gantry_left A5
#define shooter_back A3
#define shooter_front A4
#define pick_up A7
#define pick_down A6
#define latch_pin 5
#define bldc_pwm 11
#define bldc_dir 13
#define pick_pwm 3
#define pick_dir 4
#define gantry_pwm 9
#define gantry_dir A0
#define tilt_pwm 10
#define tilt_dir A1

Servo latch_s;

void setup() {
  // Starting serial communication
  Serial.begin(115200);

  pinMode(gantry_right, INPUT);
  pinMode(gantry_left, INPUT_PULLUP);
  pinMode(shooter_back, INPUT_PULLUP);
  pinMode(shooter_front, INPUT_PULLUP);
  pinMode(solenoid_pin, OUTPUT);
  pinMode(bldc_dir, OUTPUT);
  pinMode(bldc_pwm, OUTPUT);
  pinMode(pick_dir, OUTPUT);
  pinMode(pick_pwm, OUTPUT);
  pinMode(gantry_dir, OUTPUT);
  pinMode(gantry_pwm, OUTPUT);
  pinMode(tilt_pwm, OUTPUT);
  pinMode(tilt_dir, OUTPUT);
  latch_s.attach(latch_pin);

  // Running shoot sequence at startup for bringing shooting mechanism to initial state
  shoot_seq();
}

// For serial communication
char msg_str[100];
char str_buff[7];
int idx;

// For bldc motor
int bldc_pow = 0;

// For picking
int pick = 0;

// For tilting
int tilt = 0;

// Function to move stepper motor
void move_bldc() {
  if (bldc_pow == -1 && digitalRead(shooter_back)!=0) {
    digitalWrite(bldc_dir, 0);
    analogWrite(bldc_pwm, 255);
  } else if (bldc_pow == 1 && digitalRead(shooter_front)!=0) {
    digitalWrite(bldc_dir, 1);
    analogWrite(bldc_pwm, 255);
  } else {
    analogWrite(bldc_pwm, 0);
  }
}

// Function to move picking plate
void move_picker() {
  if (pick == 1 && analogRead(pick_up) != 0) {
    digitalWrite(pick_dir, LOW);
    analogWrite(pick_pwm, 100);
  } else if (pick == -1 && analogRead(pick_down) != 0) {
    digitalWrite(pick_dir, HIGH);
    analogWrite(pick_pwm, 255);
  } else {
    analogWrite(pick_pwm, 0);
  }
}

// Function to tilt shooting mechanism
void move_tilt() {
  if (tilt == 1) {
    digitalWrite(tilt_dir, LOW);
    digitalWrite(tilt_pwm, 1);
  } else if (tilt == -1) {
    digitalWrite(tilt_dir, HIGH);
    digitalWrite(tilt_pwm, 1);
  } else {
    digitalWrite(tilt_pwm, 0);
  }
}

// Function for reloading sequence
void reload_seq() {
  // Move gantry left
  while (digitalRead(gantry_left) != 0) {
    digitalWrite(gantry_dir, 1);
    digitalWrite(gantry_pwm, 1);
  }
  digitalWrite(gantry_pwm, 0);
  //Move picking plate up
  while (analogRead(pick_up) != 0) {
    digitalWrite(pick_dir, LOW);
    analogWrite(pick_pwm, 100);
  }
  analogWrite(pick_pwm, 0);
  //Move gantry right
  while (analogRead(gantry_right) >= 5) {
    digitalWrite(gantry_dir, 0);
    digitalWrite(gantry_pwm, 1);
  }
  digitalWrite(gantry_pwm, 0);
  //Move gantry left
  while (digitalRead(gantry_left) != 0) {
    digitalWrite(gantry_dir, 1);
    digitalWrite(gantry_pwm, 1);
  }
  digitalWrite(gantry_pwm, 0);
}

// Function to run shooting sequence
void shoot_seq() {
  // Release shooting plate and ring
  digitalWrite(solenoid_pin, LOW);
  delay(500);
  latch_s.write(0);
  delay(1000);
  digitalWrite(solenoid_pin, HIGH);
  // Move forward and latch to shooting plate
  while (digitalRead(shooter_front) != 0) {
    bldc_pow = 1;
    move_bldc();
  }
  bldc_pow = 0;
  move_bldc();
  delay(300);
  latch_s.write(45);
  delay(1000);
}


void loop() {
  // Reading data from esp07
  while (Serial.available()) {
    msg_str[idx] = Serial.read();
    if (msg_str[idx] == LF) {
      msg_str[idx - 1] = 0;
      idx = 0;
      break;
    }
    idx++;
  }

  int i = 4;
  int j = 0;
  int var = 1;

  // Splitting the data
  if (msg_str[0] == 'v' && msg_str[1] == 'a' && msg_str[2] == 'l') {
    while (var <= 7) {
      if (msg_str[i] == ' ' || msg_str[i] == 0) {
        str_buff[j] = 0;
        switch (var) {
          case 4:
            pick = atoi(str_buff);
            break;
          case 5:
            bldc_pow = atoi(str_buff);
            break;
          case 6:
            tilt = atoi(str_buff);
            break;
        }
        var++;
        j = -1;
      } else {
        str_buff[j] = msg_str[i];
      }
      i++;
      j++;
    }
  } else if (msg_str[0] == 's' && msg_str[1] == 'h' && msg_str[2] == 't') {
    shoot_seq();
  } else if (msg_str[0] == 'r' && msg_str[1] == 'e' && msg_str[2] == 'l') {
    reload_seq();
  }
  move_picker();
  move_bldc();
  move_tilt();
}