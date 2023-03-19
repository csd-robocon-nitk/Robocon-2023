#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define LF 0x0A

char msg_str[100];
char str_buff[7];
int idx;
String str;

float z = 0;
sensors_event_t a, g, temp;

float err_z,e_angle = 0,curr_a = 0;

Adafruit_MPU6050 mpu;

long int m_t_prev, m_t_curr;

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
    analogWrite(PWM, (int)fabs(pwr));
    digitalWrite(DIR, dir);
    t_prev = millis();
  }
};

Motor motors[3];

float mat[3][3] = { { 0, -2, 0.8 }, { 1.1547, 1, 0.8 }, { -1.1547, 1, 0.8 } };

float vel[3] = { 0, 0, 0 };

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  mpu.enableSleep(false);
  mpu.enableCycle(false);
  mpu.begin();
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("");
  delay(1000);
  Serial.println("Calibrating....Do not move mpu6050");
  mpu.getEvent(&a, &g, &temp);
  err_z = g.gyro.z;
  Serial.println("Done");
  Serial.println("");

  motors[0] = Motor(5, 4, 2);
  motors[1] = Motor(7, 6, 3);
  motors[2] = Motor(9, 8, 18);

  attachInterrupt(digitalPinToInterrupt(motors[0].ENC), update_motor_0, RISING);
  attachInterrupt(digitalPinToInterrupt(motors[1].ENC), update_motor_1, RISING);
  attachInterrupt(digitalPinToInterrupt(motors[2].ENC), update_motor_2, RISING);
  digitalWrite(LED_BUILTIN, 1);

  m_t_prev = millis();
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
    while (var <= 3) {
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
  multiply();
  Serial.print(motors[0].rpm);
  Serial.print(" ");
  Serial.print(motors[1].rpm);
  Serial.print(" ");
  Serial.println(motors[2].rpm);
  move_motor(&motors[0]);
  move_motor(&motors[1]);
  move_motor(&motors[2]);
  //Serial.println();
}

void multiply() {
  m_t_curr = millis();
  if(m_t_curr-m_t_prev>=10){
    mpu.getEvent(&a, &g, &temp);
    float vel_z = g.gyro.z  - err_z;  
    float cur_z = vel_z*0.573;
    z = (cur_z>=0.03 || cur_z<=-0.03)?(z + cur_z*1.36):z;
  }
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
  m->t_curr = millis();
  if (m->t_curr - m->t_prev >= 200) {
    m->rpm = m->count * 10;
    m->e = abs(m->rpm_tar) - m->rpm;
    m->e_int = m->e_int + (m->e * 0.2);
    m->pwr = m->pwr + 0.05*m->e;// 1*m->e_int;
    m->count = 0;
    m->t_prev = millis();
  }
  if (m->rpm_tar > 0) {
    m->dir = 0;
  } else if (m->rpm_tar < 0) {
    m->dir = 1;
  } 
  digitalWrite(m->DIR, m->dir);
  analogWrite(m->PWM, (int)fabs(m->pwr));
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