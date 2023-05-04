#include <TimerOne.h>
#include <Servo.h>
#define LF 0x0A


//////////////// Gantry Pins ///////////////////////////////////////////
int gantry_pwm = 10;
int gantry_dir = A1;
int gantry_lmt_r = A5;
int gantry_lmt_l = A4;
////////////////////////////////////////////////////////////////////////

////////////////// Picking Pins ////////////////////////////////////////
int lift_pwm = 3;
int lift_dir = 4;
int lift_lmt_u = A2;
int lift_lmt_d = A3;
/////////////////////////////////////////////////////////////////////////

///////////////////////// Shooting pins //////////////////////////////////
int push_lmt_front = A6;
int push_lmt_back = A7;
int push_pwm = 11;
int push_dir = 13;
//////////////////////////////////////////////////////////////////////////

///////////////////////// Tilt pins //////////////////////////////////
int tilt_pwm = 9;
int tilt_dir = A0;
//////////////////////////////////////////////////////////////////////

//////////////////////// Solenoid pins //////////////////////////////
int relay_pin = 6;
/////////////////////////////////////////////////////////////////////

////////////////////// Intermediate / Status variables ///////////////////
char msg_str[100];
char str_buff[7];
int idx;

int lift_sts;
int reload_trgr, prev_reload_trgr;

int curr_state=0;
int prev_state=0;
int l_angle=0; // the angle of latch

int tilt_sts = 0;
int step_pow = 0;
int shoot = 0;
/////////////////////////////////////////////////////////////////////////

/////////////////// Define Class Motor //////////////////////////////////
class Motor {
public:
  int PWM;
  int DIR;
  int ENCA;
  int ENCB;
  int count;
  float e = 0;
  float pwr = 0;
  short int dir = 0;
  float rpm_tar;
  long int t_prev, t_curr;
  float e_int = 0;
  float rpm = 0.00;
  float kp = 0;
  float ki = 0;

  Motor() {}
  Motor(int pwm, int dir) {
    PWM = pwm;
    DIR = dir;
    count = 0;
    pinMode(PWM, OUTPUT);
    pinMode(DIR, OUTPUT);
    analogWrite(PWM, LOW);
    digitalWrite(DIR, LOW);
  }
};
///////////////////////////////////////////////////////////////////////

////////////////// Initialising The Motors ////////////////////////////;
Motor tilt, stepper, gantry, lift;
Servo latch;
////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////// Run Stepper Function /////////////////////////////////////////////////////////////////
void run_stepper() {
  if((digitalRead(push_lmt_front)==0 && digitalRead(stepper.DIR)==1) || (digitalRead(push_lmt_back)==0 && digitalRead(stepper.DIR)==0) || step_pow == 0) {
    digitalWrite(stepper.PWM, 0);
    digitalWrite(stepper.PWM, 0);
  }else{
    digitalWrite(stepper.PWM, 0);
    digitalWrite(stepper.PWM, 1);
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////// Setup function ////////////////////////////////////
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(115200);
  latch.attach(44);

  latch.write(l_angle);

  tilt = Motor(tilt_pwm, tilt_dir);
  stepper = Motor(push_pwm, push_dir);
  gantry = Motor(gantry_pwm, gantry_dir);
  lift = Motor(lift_pwm, lift_dir);

  pinMode(gantry_lmt_r, INPUT_PULLUP);
  pinMode(gantry_lmt_l, INPUT_PULLUP);
  pinMode(lift_lmt_u, INPUT_PULLUP);
  pinMode(lift_lmt_d, INPUT_PULLUP);
  pinMode(push_lmt_front, INPUT_PULLUP);
  pinMode(push_lmt_back, INPUT_PULLUP);
  pinMode(relay_pin, OUTPUT);


  Timer1.attachInterrupt(run_stepper);
}
/////////////////////////////////////////////////////////////////////////

////////////////////////// Move Stepper Function ///////////////////////
void move_stepper() {
  if (step_pow != 0) {
    if (step_pow == -1) {
      digitalWrite(stepper.DIR, 0);
      }
    } else if (step_pow == 1) {
      digitalWrite(stepper.DIR, 1);
    }
  }
}
/////////////////////////////////////////////////////////////////////////

///////////////////////// Loop function //////////////////////////////////
void loop() {
  //************ PS2 Reciever *****************************************//
  while (Serial.available()) {
    msg_str[idx] = Serial.read();
    if (msg_str[idx] == LF) {
      msg_str[idx - 1] = 0;
      idx = 0;
      break;
    }
    idx++;
  }
  //Serial.println(msg_str);
  int i = 4;
  int j = 0;
  int var = 1;

  if (msg_str[0] == 'v' && msg_str[1] == 'a' && msg_str[2] == 'l') {
    while (var <= 9) {
      if (msg_str[i] == ' ' || msg_str[i] == 0) {
        str_buff[j] = 0;
        switch (var) {
          case 4:
            lift_sts = atoi(str_buff);
            break;
          case 5:
            tilt_sts = atoi(str_buff);
            break;
          case 6:
            reload_trgr = atoi(str_buff);
            break;
          case 7:
            step_pow = atoi(str_buff);
            break;
          case 8:
            shoot = atoi(str_buff);
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
  }
  //*******************************************************************//


  if (reload_trgr == 1 && prev_reload_trgr != reload_trgr) {
    while (digitalRead(gantry_lmt_l)) {
      digitalWrite(gantry_dir, 0);
      digitalWrite(gantry_pwm, 1);
      delay(1);
    }
    digitalWrite(gantry_pwm, 0);
    while (digitalRead(lift_lmt_u)) {
      digitalWrite(lift_dir, 0);
      digitalWrite(lift_pwm, 1);
      digitalWrite(relay_pin,HIGH);
      delay(1);
    }
    digitalWrite(lift_pwm, 0);
    while (digitalRead(gantry_lmt_r)) {
      digitalWrite(gantry_dir, 1);
      digitalWrite(gantry_pwm, 1);
      delay(1);
    }
    digitalWrite(gantry_pwm, 0);
    while (digitalRead(gantry_lmt_l)) {
      digitalWrite(gantry_dir, 0);
      digitalWrite(gantry_pwm, 1);
      delay(1);
    }
    
    digitalWrite(gantry_pwm, 0);
    digitalWrite(relay_pin,LOW);
    delay(300);
  }
  prev_reload_trgr = reload_trgr;

  if(lift_sts != 0)
  {
    if(lift_sts == 1)
      digitalWrite(lift_dir, 0);
    else
      digitalWrite(lift_dir, 1);
    digitalWrite(lift_pwm, 1);
  }
  else
  {
    digitalWrite(lift_pwm, 0);
  }
  
}
//////////////////////////////////////////////////////////////////////