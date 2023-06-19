#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

float x = 0;
float y = 0;
float z = 0;
float err_x,err_y,err_z;
Adafruit_MPU6050 mpu;
int tar = 90;
float e,e_sum = 0,e_der = 0;
float e_prev = tar;

struct Motor {
  int PWM;
  int DIR;
};

Motor motors[4];

void setup() {
  Serial.begin(115200);
  Serial.println("Setting Up Motors");
  motors[0].PWM = 5, motors[0].DIR = 4;
  motors[1].PWM = 7, motors[1].DIR = 6;
  motors[2].PWM = 9, motors[2].DIR = 8;
  motors[3].PWM = 11, motors[3].DIR = 10;
  for (int i = 0; i < 4; i++) {
    pinMode(motors[i].PWM, OUTPUT);
    pinMode(motors[i].DIR, OUTPUT);
  }
  Serial.println("Finding Adafruit MPU6050");
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
  }
  Serial.println("MPU6050 Found!");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("");
  delay(1000);
  Serial.println("Calibrating....Do not move mpu6050");
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  err_x = g.gyro.x;
  err_y = g.gyro.y;
  err_z = g.gyro.z;
  Serial.println("Done");
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  float vel_x = g.gyro.x - err_x;
  float vel_y = g.gyro.y - err_y;
  float vel_z = g.gyro.z  - err_z;
  float cur_x = vel_x*0.573;  
  float cur_y = vel_y*0.573;  
  float cur_z = vel_z*0.573;   
  x = (cur_x>=0.03 || cur_x<=-0.03)?(x + cur_x*1.36):x;
  y = (cur_y>=0.03 || cur_y<=-0.03)?(y + cur_y*1.36):y;
  z = (cur_z>=0.03 || cur_z<=-0.03)?(z + cur_z*1.36):z;
  Serial.print(z);
  Serial.print(" ");
  Serial.print(e);
  Serial.print(" ");
  Serial.print(e_sum);
  Serial.print(" ");
  Serial.print(e_der);
  Serial.print(" ");
  e = tar - z;
  e_sum = e_sum + e*0.05;
  e_der = (e - e_prev)/0.01;
  int pwm = 25 * e + 4 * e_sum - 2 * e_der;
  if(pwm>8000)
    pwm = 8000;
  else if(pwm<-8000)
    pwm = -8000;
  pwm = map(pwm,-8000,8000,-120,120);  
  Serial.print(pwm);
  Serial.println(" ");
  e_prev = e;
  run_motor(pwm);
  delay(10);
}

void run_motor(int pwm)
{  
  for(int i = 0;i<4;i++){
    if(pwm<0)
      digitalWrite(motors[i].DIR, 1);
    else
      digitalWrite(motors[i].DIR, 0);
    analogWrite(motors[i].PWM, abs(pwm));
  }  
}