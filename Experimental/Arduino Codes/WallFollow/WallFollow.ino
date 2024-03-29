// Program to make the robot maintain a specific distance from the wall

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "Ultrasonic.h"
#define LF 0x0A

float z = 0;

// Ultrasonic sensor connected to pin 53
Ultrasonic ultrasonic(53);

// Distance to be maintained from the wall in cm
int Tar_Dis = 50;

// A permissible error of  +/- 2cm
int const_error = 2;

// PID controller for wall follow
int dist_read;
float errorD = 0, integral = 0, derivative = 0, output = 0, previousErrorD = 0;
float KpD = 0.7, KiD = 0.69, KdD = 0.5;

// PID controller for angle correction
float errorw = 0, integralw = 0, derivativew = 0, outputw = 0, previousErrorw = 0;
float Kpw = 0.8, Kiw = 0.4, Kdw = 0.2;

char msg_str[100];
char str_buff[7];
int idx;

// Conversion matrix for holonomic drive
float mat[3][3] = {{0, -1.3334, 0.8}, {1.1547, 0.6667, 0.8}, {-1.1547, 0.6667, 0.8}};

// A class defining alll features of a motor
class Motor
{
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
  Motor(int pwm, int dir)
  {
    PWM = pwm;
    DIR = dir;
    count = 0;
    pinMode(PWM, OUTPUT);
    pinMode(DIR, OUTPUT);
    analogWrite(PWM, LOW);
    digitalWrite(DIR, LOW);
  }
  Motor(int pwm, int dir, int enca, int encb, float kp_value, float ki_value)
  {
    PWM = pwm;
    DIR = dir;
    ENCA = enca;
    ENCB = encb;
    kp = kp_value;
    ki = ki_value;
    count = 0;
    pinMode(ENCA, INPUT);
    pinMode(ENCB, INPUT);
    pinMode(PWM, OUTPUT);
    pinMode(DIR, OUTPUT);
    analogWrite(PWM, (int)fabs(pwr));
    digitalWrite(DIR, dir);
    t_prev = millis();
  }
};

Motor motors[3];
Motor lift;

// Interrup handlers to update the count of the motor encoders
void update_count_0()
{
  if (digitalRead(motors[0].ENCB) != 1)
  {
    motors[0].count++;
  }
  else
  {
    motors[0].count--;
  }
}

void update_count_1()
{
  if (digitalRead(motors[1].ENCB) != 1)
  {
    motors[1].count++;
  }
  else
  {
    motors[1].count--;
  }
}

void update_count_2()
{
  if (digitalRead(motors[2].ENCB) != 1)
  {
    motors[2].count++;
  }
  else
  {
    motors[2].count--;
  }
}

float vel[3] = {0, 0, 0};
float glb_vel[3] = {0, 0, 0};

float lift_sts = 0;

void setup()
{
  delay(20000);
  Serial.begin(115200);
  Serial2.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  // Creating 3 motor objects with the following pin and kp, ki values
  motors[0] = Motor(5, 4, 2, 10, 0.5, 1.5);
  motors[1] = Motor(7, 6, 3, 11, 0.5, 1.5);
  motors[2] = Motor(9, 8, 18, 12, 0.5, 1.5);

  lift = Motor(33, 31);

  // Using interrupts for encoders
  attachInterrupt(digitalPinToInterrupt(motors[0].ENCA), update_count_0, RISING);
  attachInterrupt(digitalPinToInterrupt(motors[1].ENCA), update_count_1, RISING);
  attachInterrupt(digitalPinToInterrupt(motors[2].ENCA), update_count_2, RISING);

  digitalWrite(LED_BUILTIN, 1);
}

// Function for wall following algorithm
float pid_distance(int distance)
{
  if (distance - Tar_Dis > 2 || distance - Tar_Dis < -2)
  {
    errorD = Tar_Dis - distance;
    integral = integral + errorD * 0.01;
    derivative = (errorD - previousErrorD);
    output = KpD * errorD + KiD * integral + KdD * derivative;
    previousErrorD = errorD;
  }
  else
  {
    errorD = 0;
    integral = 0;
    derivative = 0;
    output = 0;
  }
  if (output > 100)
  {
    output = 100;
  }
  if (output < -100)
  {
    output = -100;
  }
  return output;
}

// Function for angle correction
float pid_angle(int angle)
{
  if (z > 3 || z < -3)
  {
    errorw = -angle;
    integralw = integralw + errorw * 0.01;
    derivativew = (errorw - previousErrorw);
    outputw = Kpw * errorw + Kiw * integralw + Kdw * derivativew;
    previousErrorw = errorw;
  }
  else
  {
    errorw = 0;
    integralw = 0;
    derivativew = 0;
    outputw = 0;
  }
  return outputw;
}

void loop()
{
  while (Serial2.available())
  {
    // Read command from wifi via ESP07
    msg_str[idx] = Serial2.read();
    if (msg_str[idx] == LF)
    {
      msg_str[idx - 1] = 0;
      idx = 0;
      break;
    }
    idx++;
  }
  int i = 4;
  int j = 0;
  int var = 1;
  if (msg_str[0] == 'v' && msg_str[1] == 'a' && msg_str[2] == 'l')
  {
    while (var <= 5)
    {
      if (msg_str[i] == ' ' || msg_str[i] == 0)
      {
        str_buff[j] = 0;
        switch (var)
        {
        case 1:
          glb_vel[0] = atof(str_buff);
          break;
        case 2:
          glb_vel[1] = atof(str_buff);
          break;
        case 3:
          glb_vel[2] = -1 * atof(str_buff);
          break;
        case 4:
          lift_sts = atof(str_buff);
          break;
        case 5:
          z = atof(str_buff);
          break;
        }
        var++;
        j = -1;
      }
      else
      {
        str_buff[j] = msg_str[i];
      }
      i++;
      j++;
    }
  }

  // For testing purposes, add a constant linear velocity
  glb_vel[1] = -50;
  dist_read = ultrasonic.MeasureInCentimeters();

  // Update the speed of robot according to distance from the wall
  glb_vel[0] = -1 * pid_distance(dist_read);

  // Convert RPM values to motor RPM
  multiply();

  Serial.print(motors[0].rpm_tar);
  Serial.print(" ");
  Serial.print(motors[1].rpm_tar);
  Serial.print(" ");
  Serial.print(motors[2].rpm_tar);
  Serial.print(" ");
  Serial.print(z);
  Serial.print(" ");

  // Write pwm and dir values to motor driver
  move_motor(&motors[0]);
  move_motor(&motors[1]);
  move_motor(&motors[2]);
  Serial.print(dist_read);
  Serial.println();
}

void multiply()
{
  float angle = 0;
  vel[0] = glb_vel[0] * cos(angle) - glb_vel[1] * sin(angle);
  vel[1] = glb_vel[0] * sin(angle) + glb_vel[1] * cos(angle);
  vel[2] = pid_angle(z);
  motors[0].rpm_tar = (mat[0][0] * vel[0] + mat[0][1] * vel[1] + mat[0][2] * vel[2]);
  motors[1].rpm_tar = (mat[1][0] * vel[0] + mat[1][1] * vel[1] + mat[1][2] * vel[2]);
  motors[2].rpm_tar = (mat[2][0] * vel[0] + mat[2][1] * vel[1] + mat[2][2] * vel[2]);
}

// Function to run the motors
void move_motor(Motor *m)
{
  m->t_curr = millis();
  if (m->t_curr - m->t_prev >= 200)
  {
    m->rpm = m->count * 3;
    m->e_int = m->e_int + (m->e * 0.2);
    m->e = m->rpm_tar - m->rpm;

    m->pwr = m->kp * m->e + m->ki * m->e_int;
    if (m->rpm_tar == 0)
    {
      m->pwr = 0;
      m->e = 0;
      m->e_int = 0;
    }
    m->count = 0;
    m->t_prev = millis();
  }
  if (m->pwr >= 0)
  {
    m->dir = 0;
  }
  else
  {
    m->dir = 1;
  }
  digitalWrite(m->DIR, m->dir);
  analogWrite(m->PWM, (int)fabs(m->pwr));
}