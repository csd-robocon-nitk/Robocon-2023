#include <Servo.h>
#include <TimerOne.h>

#define LF 0x0A

#define gantry_right A2
#define gantry_left A5
#define shooter_back A3
#define shooter_front A4
#define pick_up A7
#define pick_down A6
#define solenoid_pin 6
#define latch_pin 5
#define stepper_pwm 0  //Change it
#define stepper_dir 0  //Change it
#define pick_pwm 0     //Change it
#define pick_dir 0     //Change it
#define gantry_pwm 0    //Change it
#define gantry_dir 0    //Change it
#define lift_pwm 0     //Change it
#define lift_dir 0     //Change it

Servo latch_servo;

// For serial communication
char msg_str[100];
char str_buff[7];
int idx;

// For stepper motor
int step_pow = 0;

// For picking
int pick = 0;

// For lifting
int lift = 0; 

// Function to manually generate pulse for stepper motor
void run_stepper() {
  if ((digitalRead(shooter_back) == 0 && digitalRead(stepper_dir) == 1) || (digitalRead(shooter_front) == 0 && digitalRead(stepper_dir) == 0) || step_pow == 0) {
    digitalWrite(stepper_pwm, 0);
    digitalWrite(stepper_pwm, 0);
  } else {
    digitalWrite(stepper_pwm, 0);
    digitalWrite(stepper_pwm, 1);
  }
}

// Function to move stepper motor
void move_stepper() {
  if (step_pow != 0) {
    if (step_pow == -1) {
      digitalWrite(stepper_dir, 0);
    } else if (step_pow == 1) {
      digitalWrite(stepper_dir, 1);
    }
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

// Function to lift shooting mechanism
void move_lift() {
  if (lift == 1) {
    digitalWrite(lift_dir, LOW);
    digitalWrite(lift_pwm, 1);
  } else if (lift == -1) {
    digitalWrite(lift_dir, HIGH);
    digitalWrite(lift_pwm, 1);
  } else {
    digitalWrite(lift_pwm, 0);
  }
}

// Function for reloading sequence
void reload_seq() {
  while (digitalRead(gantry_left) != 0) {
    digitalWrite(gantry_dir, 1);
    digitalWrite(gantry_pwm, 1);
  }
  digitalWrite(gantry_pwm, 0);
  digitalWrite(solenoid_pin, HIGH);
  while (analogRead(pick_up) != 0) {
    //Move picking plate up and turn on solenoid
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
  while (digitalRead(gantry_left) != 0) {
    //Move gantry left and switch of solenoid
    digitalWrite(gantry_dir, 1);
    digitalWrite(gantry_pwm, 1);
  }
  digitalWrite(gantry_pwm, 0);
  digitalWrite(solenoid_pin, LOW);
}

// Function to run shooting sequence
void shoot_seq() {
  digitalWrite(solenoid_pin, HIGH);
  delay(500);
  latch_servo.write(0);
  while (digitalRead(shooter_front != 0)) {
    step_pow = 1;
    move_stepper();
  }
  step_pow = 0;
  move_stepper();
  latch_servo.write(45);
  digitalWrite(solenoid_pin, LOW);
}

void setup() {
  Serial.begin(115200);
  pinMode(gantry_right, INPUT_PULLUP);
  pinMode(gantry_left, INPUT_PULLUP);
  pinMode(shooter_back, INPUT_PULLUP);
  pinMode(shooter_front, INPUT_PULLUP);
  pinMode(solenoid_pin, OUTPUT);
  pinMode(stepper_dir, OUTPUT);
  pinMode(stepper_pwm, OUTPUT);
  pinMode(pick_dir, OUTPUT);
  pinMode(pick_pwm, OUTPUT);
  pinMode(gantry_dir, OUTPUT);
  pinMode(gantry_pwm, OUTPUT);
  pinMode(lift_pwm, OUTPUT);
  pinMode(lift_dir, OUTPUT);
  latch_servo.attach(latch_pin);
  Timer1.initialize(350);
  Timer1.attachInterrupt(run_stepper);
  shoot_seq();
}

void loop() {
  //Reading data from esp07
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
          case 5:
            step_pow = atoi(str_buff);
            break;
          case 6:
            lift = atoi(str_buff);
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
  move_stepper();
  move_lift();
}