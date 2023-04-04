#include "MCP4725.h"

#define LF 0x0A

MCP4725 MCP(0x61);

char msg_str[100];
char str_buff[7];
int idx;
String str;

float z = 0;

float e_angle = 0,curr_a = 0;

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

void loop() {
  // while (Serial2.available()) {
  //   msg_str[idx] = Serial2.read();
  //   if (msg_str[idx] == LF) {
  //     msg_str[idx - 1] = 0;
  //     idx = 0;
  //     break;
  //   }
  //   idx++;
  // }
  // int i = 4;
  // int j = 0;
  // int var = 1;
  // if (msg_str[0] == 'v' && msg_str[1] == 'a' && msg_str[2] == 'l') {
  //   while (var <= 5) {
  //     if (msg_str[i] == ' ' || msg_str[i] == 0) {
  //       str_buff[j] = 0;
  //       switch (var) {
  //         case 1:
  //           vel[0] = -1*atof(str_buff);
  //           break;
  //         case 2:
  //           vel[1] = -1*atof(str_buff);
  //           break;
  //         case 3:
  //           vel[2] = -1*atof(str_buff);
  //           break;
  //         case 4:
  //           break;
  //         case 5:
  //           z = atof(str_buff);
  //           break;
  //       }
  //       var++;
  //       j = -1;
  //     } else {
  //       str_buff[j] = msg_str[i];
  //     }
  //     i++;
  //     j++;
  //   }
  // }
  vel[0] = 100;
  multiply();
  Serial.print(motors[0].rpm_tar);
  Serial.print(" ");
  Serial.print(motors[1].rpm_tar);
  Serial.print(" ");
  Serial.print(motors[2].rpm_tar);
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
  if(vel[2]==0){
    e_angle = curr_a - z;
    vel[2] = e_angle*2;
  }
  else {
    curr_a = z;
  }
  motors[0].rpm_tar = (mat[0][0] * vel[0] + mat[0][1] * vel[1] + mat[0][2] * vel[2])/1.5;
  motors[1].rpm_tar = (mat[1][0] * vel[0] + mat[1][1] * vel[1] + mat[1][2] * vel[2])/1.5;
  motors[2].rpm_tar = (mat[2][0] * vel[0] + mat[2][1] * vel[1] + mat[2][2] * vel[2])/1.5;
}

void move_motor(Motor *m) {
  // m->t_curr = millis();
  // if (m->t_curr - m->t_prev >= 200) {
  //   m->rpm = m->count * 10;
  //   m->e = abs(m->rpm_tar) - m->rpm;
  //   m->e_int = m->e_int + (m->e * 0.2);
  //   m->pwr = m->pwr + 0.05*m->e;// 1*m->e_int;
  //   if (m->rpm_tar == 0) {
  //     m->pwr = 0;
  //     m->e = 0;
  //     m->e_int = 0;
  //   }
  //   m->count = 0;
  //   m->t_prev = millis();
  // }
  m->pwr = map(m->rpm_tar, -200, 200, -255, 255);
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