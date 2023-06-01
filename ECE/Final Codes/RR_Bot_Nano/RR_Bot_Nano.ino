#include <Servo.h>

#define LF 0x0A

// Pins for each component
#define gantry_right A2
#define gantry_left A3
#define shooter_back A4
#define shooter_front A7
#define pick_up A5
#define pick_down A6
#define esc_pin 5
#define servo_pin 6
#define push_pwm 12
#define push_dir A0
#define pick_pwm 3
#define pick_dir 4
#define gantry_pwm 8
#define gantry_dir 13
#define tilt_pwm 10
#define tilt_dir A1

Servo g_servo, esc;

void setup() {
  Serial.begin(115200);
  pinMode(gantry_right, INPUT_PULLUP);
  pinMode(gantry_left, INPUT_PULLUP);
  pinMode(shooter_back, INPUT_PULLUP);
  pinMode(shooter_front, INPUT_PULLUP);
  pinMode(pick_up, INPUT_PULLUP);
  pinMode(pick_down, INPUT_PULLUP);
  pinMode(pick_dir, OUTPUT);
  pinMode(pick_pwm, OUTPUT);
  pinMode(gantry_dir, OUTPUT);
  pinMode(gantry_pwm, OUTPUT);
  pinMode(tilt_pwm, OUTPUT);
  pinMode(tilt_dir, OUTPUT);
  pinMode(push_pwm, OUTPUT);
  pinMode(push_dir, OUTPUT);
  g_servo.attach(servo_pin);
  esc.attach(esc_pin, 1000, 2000);
  // Calibrating ESC
  esc.write(180);
  delay(5000);
  esc.write(0);
  delay(5000);
  g_servo.write(180);
  shoot_seq();
}

// For serial communication
char msg_str[100];
char str_buff[7];
int idx;

// For picking
int pick = 0;

// For tilting
int tilt = 0;

// Function to move picking plate
void move_picker() {
  if (pick == 1 && digitalRead(pick_up) != 0) {
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
    digitalWrite(tilt_dir, HIGH);
    analogWrite(tilt_pwm, 100);
  } else if (tilt == -1) {
    digitalWrite(tilt_dir, LOW);
    analogWrite(tilt_pwm, 100);
  } else {
    digitalWrite(tilt_pwm, 0);
  }
}

// Function for reloading sequence
void reload_seq() {
  while (digitalRead(gantry_left) != 0) {
    // Move gantry left
    digitalWrite(gantry_dir, 1);
    digitalWrite(gantry_pwm, 1);
  }
  digitalWrite(gantry_pwm, 0);
  while (digitalRead(pick_up) != 0) {
    //Move picking plate up
    digitalWrite(pick_dir, LOW);
    analogWrite(pick_pwm, 100);
  }
  analogWrite(pick_pwm, 0);
  while (digitalRead(gantry_right) != 0) {
    //Move gantry right
    digitalWrite(gantry_dir, 0);
    digitalWrite(gantry_pwm, 1);
  }
  digitalWrite(gantry_pwm, 0);
  //Move gantry servo
  g_servo.write(20);
  delay(1000);
  g_servo.write(180);
  delay(1000);
  while (digitalRead(gantry_left) != 0) {
    //Move gantry left
    digitalWrite(gantry_dir, 1);
    digitalWrite(gantry_pwm, 1);
  }
  digitalWrite(gantry_pwm, 0);
}

// Function to run shooting sequence
void shoot_seq() {
  // Turn on BLDC
  esc.write(180);
  delay(500);

  // Push ring forward
  while (analogRead(shooter_front) != 0) {
    digitalWrite(push_dir, LOW);
    digitalWrite(push_pwm, HIGH);
  }
  digitalWrite(push_pwm, LOW);
  while (digitalRead(shooter_back) != 0) {
    digitalWrite(push_dir, HIGH);
    digitalWrite(push_pwm, HIGH);
  }
  digitalWrite(push_pwm, LOW);

  // Switch off BLDC
  esc.write(0);
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
    while (var <= 10) {
      if (msg_str[i] == ' ' || msg_str[i] == 0) {
        str_buff[j] = 0;
        switch (var) {
          case 4:
            pick = atoi(str_buff);
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
  move_tilt();
}