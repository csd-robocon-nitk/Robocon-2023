#include "MCP4725.h"

#define LF 0x0A
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "Ultrasonic.h"

MCP4725 MCP(0x61);

char msg_str[100];
char str_buff[7];
int idx;
String str;

float z = 0;

Ultrasonic ultrasonic(53); 
int INPUT_DISTANCE = 50;
int CONSTANT_OMEGA = 0;
int const_error=2;
int dist_read;
float errorD= 0, integral =0, derivative=0, output=0, previousErrorD=0;
float KpD = 0.7, KiD = 0.4 ,KdD = 0.06;

float errorw= 0, integralw =0, derivativew=0, outputw=0, previousErrorw=0;
float Kpw = 0.5, Kiw = 0.2 ,Kdw = 0;


//float e_angle = 0,curr_a = 0;

class Motor {
public:
  int PWM;
  int DIR;
  int ENC;
  int count;
  float e;
  float pwr = 0;
  short int dir = 0;
  float rpm_tar;
  long int t_prev, t_curr;
  float e_int;
  float rpm = 0.00;

  Motor() {}
  Motor(int pwm, int dir, int enc) {
    PWM = pwm;
    DIR = dir;
    ENC = enc;
    count = 0;
    pinMode(ENC, INPUT);
    pinMode(PWM, OUTPUT);
    pinMode(DIR, OUTPUT);
    digitalWrite(PWM, 0);
    digitalWrite(DIR, 0);
    t_prev = millis();
  }
};

Motor motors[3];

float mat[3][3] = { { 0, -2, 0.8 }, { 1.1547, 1, 0.8 }, { -1.1547, 1, 0.8 } };

float vel[3] = { 0, 0, 0 };

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
  MCP.begin();
  pinMode(LED_BUILTIN, OUTPUT);

  motors[0] = Motor(33, 4, 2);
  motors[1] = Motor(38, 6, 3);
  motors[2] = Motor(39, 8, 18);

  attachInterrupt(digitalPinToInterrupt(motors[0].ENC), update_motor_0, RISING);
  attachInterrupt(digitalPinToInterrupt(motors[1].ENC), update_motor_1, RISING);
  attachInterrupt(digitalPinToInterrupt(motors[2].ENC), update_motor_2, RISING);
  digitalWrite(LED_BUILTIN, 1);
}

float pid_distance(int distance){
  if (distance - INPUT_DISTANCE >2 || distance - INPUT_DISTANCE <-2){
   //Serial.print("Error ");
   errorD = INPUT_DISTANCE - distance;
   //Serial.print( errorD);
   integral = integral + errorD * 0.01;
   derivative = (errorD - previousErrorD);
   output = KpD*errorD + KiD*integral + KdD*derivative;
   previousErrorD  = errorD;
  }
  else{
  errorD=0;
  integral = 0;
  derivative = 0;
  output = 0;
  }
  if (output> 100)
  {
    output = 100;
  }
  if (output < -100)
  {
    output = -100;
  }
  //Serial.print("Output ");
  //Serial.print(output);
  
  return output;
}

float pid_angle(int angle){
   if(z>3 || z<-3){
    errorw = -angle;
   integralw = integralw + errorw * 0.01;
   derivativew = (errorw - previousErrorw);
   outputw = Kpw*errorw + Kiw*integralw + Kdw*derivativew;
   previousErrorw  = errorw;
  }
  else{
  errorw=0;
  integralw = 0;
  derivativew = 0;
  outputw = 0;
  }
  return outputw;
}


void loop() {
  while (Serial2.available()) {
    msg_str[idx] = Serial2.read();
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
  if (msg_str[0] == 'v' && msg_str[1] == 'a' && msg_str[2] == 'l') {
    while (var <= 5) {
      if (msg_str[i] == ' ' || msg_str[i] == 0) {
        str_buff[j] = 0;
        switch (var) {
          case 1:
            vel[0] = -1*atof(str_buff);
            break;
          case 2:
            vel[1] = -1*atof(str_buff);
            break;
          case 3:
            vel[2] = -1*atof(str_buff);
            break;
          case 4:
            break;
          case 5:
            z = atof(str_buff);
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
  glb_vel[1] = 0;
  dist_read = ultrasonic.MeasureInCentimeters();
  glb_vel[0] = 0; //pid_distance(dist_read);
  multiply();
  Serial.print(motors[0].rpm);
  Serial.print(" ");
  Serial.print(motors[1].rpm);
  Serial.print(" ");
  Serial.print(motors[2].rpm);
  Serial.print(" ");
  Serial.println(z);
  digitalWrite(motors[0].PWM, HIGH);
  digitalWrite(motors[1].PWM, LOW);
  digitalWrite(motors[2].PWM, LOW);
  move_motor(&motors[0]);
  digitalWrite(motors[0].PWM, LOW);
  digitalWrite(motors[1].PWM, HIGH);
  digitalWrite(motors[2].PWM, LOW);
  move_motor(&motors[1]);
  digitalWrite(motors[0].PWM, LOW);
  digitalWrite(motors[1].PWM, LOW);
  digitalWrite(motors[2].PWM, HIGH);
  move_motor(&motors[2]);
}

void multiply() {
  /*if(vel[2]==0){
    e_angle = curr_a - z;
    vel[2] = e_angle*2;
  }
  else {
    curr_a = z;
  }*/
  vel[0] = glb_vel[0] * cos(angle) - glb_vel[1] * sin(angle);
  vel[1] = glb_vel[0] * sin(angle) + glb_vel[1] * cos(angle);
  vel[2] = pid_angle(z);
  motors[0].rpm_tar = (mat[0][0] * vel[0] + mat[0][1] * vel[1] + mat[0][2] * vel[2])/1.5;
  motors[1].rpm_tar = (mat[1][0] * vel[0] + mat[1][1] * vel[1] + mat[1][2] * vel[2])/1.5;
  motors[2].rpm_tar = (mat[2][0] * vel[0] + mat[2][1] * vel[1] + mat[2][2] * vel[2])/1.5;
}

void move_motor(Motor *m) {
  m->t_curr = millis();
  if (m->t_curr - m->t_prev >= 200) {
    m->rpm = m->count * 10;
    m->e = abs(m->rpm_tar) - m->rpm;
    m->e_int = m->e_int + (m->e * 0.2);
    m->pwr = m->pwr + 0.05*m->e;// 1*m->e_int;
    if (m->rpm_tar == 0) {
      m->pwr = 0;
      m->e = 0;
      m->e_int = 0;
    }
    m->count = 0;
    m->t_prev = millis();
  }
  if (m->rpm_tar > 0) {
    m->dir = 0;
  } else if (m->rpm_tar < 0) {
    m->dir = 1;
  } 
  digitalWrite(m->DIR, m->dir);
  MCP.setValue((int)(fabs(m->pwr)*4096/255));
}

void update_motor_0(){
  motors[0].count++;
}

void update_motor_1(){
  motors[1].count++;
}

void update_motor_2(){
  motors[2].count++;
}
