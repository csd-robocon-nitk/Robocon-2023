#include "MCP4725.h"

#define LF 0x0A

MCP4725 MCP(0x61);

char msg_str[100];
char str_buff[7];
int idx;
String str;

float z = 0;
float factor = 1.5;
float e_angle = 0, curr_a = 0;
int prev_s = 0, cur_s = 0;
int p_pow = 0;

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
  Motor(int pwm, int dir) {
    PWM = pwm;
    DIR = dir;
    count = 0;
    pinMode(PWM, OUTPUT);
    pinMode(DIR, OUTPUT);
    analogWrite(PWM, LOW);
    digitalWrite(DIR, LOW);
  }
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

float mat[3][3] = { { 0, -1.8, -0.8 }, { 1.1547, 0.9, -0.8 }, { -1.1547, 0.9, -0.8 } };
float vel[3] = { 0, 0, 0 };
float glb_vel[3] = { 0, 0, 0 };

Motor tilt, push;
int tilt_sts = 0, speed = 0;

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
  MCP.begin();

  motors[0] = Motor(39, 35, 31);
  motors[1] = Motor(38, 34, 36);
  motors[2] = Motor(33, 29, 37);

  digitalWrite(motors[0].PWM, HIGH);
  digitalWrite(motors[1].PWM, LOW);
  digitalWrite(motors[2].PWM, LOW);
  MCP.setValue(0);
  delay(1);
  digitalWrite(motors[0].PWM, LOW);
  digitalWrite(motors[1].PWM, HIGH);
  digitalWrite(motors[2].PWM, LOW);
  MCP.setValue(0);
  delay(1);
  digitalWrite(motors[0].PWM, LOW);
  digitalWrite(motors[1].PWM, LOW);
  digitalWrite(motors[2].PWM, HIGH);
  MCP.setValue(0);
  delay(1);
  digitalWrite(motors[0].PWM, LOW);
  digitalWrite(motors[1].PWM, LOW);
  digitalWrite(motors[2].PWM, LOW);

  tilt = Motor(7, 40);
  push = Motor(23, 25);
}

void move_p() {
    if (p_pow == 1) {
      digitalWrite(push.DIR, 0);
      digitalWrite(push.PWM, 1);
    } 
    else if (p_pow == -1) {
      digitalWrite(push.DIR, 1);
      digitalWrite(push.PWM, 1);
    } 
    else {
      digitalWrite(push.DIR, 0);
      digitalWrite(push.PWM, 0);
    } 
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
  // Serial.println(msg_str);
  int i = 4;
  int j = 0;
  int var = 1;
  if (msg_str[0] == 'v' && msg_str[1] == 'a' && msg_str[2] == 'l') {
    while (var <= 9) {
      if (msg_str[i] == ' ' || msg_str[i] == 0) {
        str_buff[j] = 0;
        switch (var) {
          case 1:
            glb_vel[0] = -1 * atof(str_buff);
            break;
          case 2:
            glb_vel[1] = -1 * atof(str_buff);
            break;
          case 3:
            glb_vel[2] = -1 * atof(str_buff);
            break;
          case 5:
            tilt_sts = atoi(str_buff);
            break;
          case 7:
            p_pow = atoi(str_buff);
            break;
          case 8:
            speed = atoi(str_buff);
            break;
          case 9:
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
  // vel[1] = 50;
  multiply();
  Serial.print(motors[0].pwr);
  Serial.print(" ");
  Serial.print(motors[1].pwr);
  Serial.print(" ");
  Serial.print(motors[2].pwr);
  Serial.print(" ");
  Serial.println(z);
  move_motor(&motors[0]);
  move_motor(&motors[1]);
  move_motor(&motors[2]);
  digitalWrite(motors[0].PWM, LOW);
  digitalWrite(motors[1].PWM, LOW);
  digitalWrite(motors[2].PWM, HIGH);
  MCP.setValue((int)(fabs(motors[2].pwr)*4096/255)); 
  delay(1); 
  digitalWrite(motors[0].PWM, HIGH);
  digitalWrite(motors[1].PWM, LOW);
  digitalWrite(motors[2].PWM, LOW);
  MCP.setValue((int)(fabs(motors[0].pwr)*4096/255));
  delay(1);  
  digitalWrite(motors[0].PWM, LOW);
  digitalWrite(motors[1].PWM, HIGH);
  digitalWrite(motors[2].PWM, LOW);
  MCP.setValue((int)(fabs(motors[1].pwr)*4096/255));  
  delay(1); 
  digitalWrite(motors[0].PWM, LOW);
  digitalWrite(motors[1].PWM, LOW);
  digitalWrite(motors[2].PWM, LOW);
  run_motor(&tilt, tilt_sts);
  move_p();
  speed_up();
}

void speed_up() {
  cur_s = speed;
  if(cur_s == 1 && cur_s!=prev_s){
    if(factor == 1.5)
      factor = 1;
    else if(factor == 1)
      factor = 1.5;
  }
  prev_s = cur_s;
}

void multiply() {
  float angle = z;
  if (angle >= 360)
    angle = angle - 360;
  if (angle <= -360)
    angle = angle + 360;
  angle = angle * PI / 180;
  vel[0] = glb_vel[0] * cos(angle) - glb_vel[1] * sin(angle);
  vel[1] = glb_vel[0] * sin(angle) + glb_vel[1] * cos(angle);
  vel[2] = glb_vel[2];
  motors[0].rpm_tar = (mat[0][0] * vel[0] + mat[0][1] * vel[1] + mat[0][2] * vel[2])/factor;
  motors[1].rpm_tar = (mat[1][0] * vel[0] + mat[1][1] * vel[1] + mat[1][2] * vel[2])/factor;
  motors[2].rpm_tar = (mat[2][0] * vel[0] + mat[2][1] * vel[1] + mat[2][2] * vel[2])/factor;
}

void run_motor(Motor* m, int sts) {
  if (sts == 0)
    m->pwr = 0;
  else if (sts == 1) {
    m->pwr = 1;
    digitalWrite(m->DIR, 0);
  } else if (sts == -1) {
    m->pwr = 1;
    digitalWrite(m->DIR, 1);
  }
  digitalWrite(m->PWM, m->pwr);
}

void move_motor(Motor* m) {
  // m->t_curr = millis();
  // if (m->t_curr - m->t_prev >= 200) {
  //   m->rpm = m->count * 10;
  // m->e = abs(m->rpm_tar) - m->rpm;
  // m->e_int = m->e_int + (m->e * 0.2);
  // m->pwr = m->pwr + 0.05*m->e;// 1*m->e_int;
  // if (m->rpm_tar == 0) {
  //   m->pwr = 0;
  //   m->e = 0;
  //   m->e_int = 0;
  // }
  //   m->count = 0;
  //   m->t_prev = millis();
  // }
  //m->pwr = map(m->rpm_tar, -300, 300, -255, 255);
  float p = map(m->rpm_tar, -300, 300, -255, 255);
  if(m->pwr<p)
    m->pwr = m->pwr + 1;
  else if(m->pwr>p)
    m->pwr = m->pwr - 1;
  if (m->pwr > 0) {
    m->dir = 0;
  } else if (m->pwr < 0) {
    m->dir = 1;
  } 
  digitalWrite(m->DIR, m->dir);
}

void update_motor_0() {
  motors[0].count++;
}

void update_motor_1() {
  motors[1].count++;
}

void update_motor_2() {
  motors[2].count++;
}