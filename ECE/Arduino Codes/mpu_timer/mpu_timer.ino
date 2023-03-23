// Basic demo for accelerometer readings from Adafruit MPU6050

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

float x = 0;
float y = 0;
float z = 0;
sensors_event_t a, g, temp;
float vel_x, vel_y, vel_z;
float cur_x, cur_y, cur_z;

float err_x,err_y,err_z;

Adafruit_MPU6050 mpu;

void setup(void) {
  Serial.begin(115200);
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
  err_x = g.gyro.x;
  err_y = g.gyro.y;
  err_z = g.gyro.z;
  Serial.println("Done");
  Serial.println("");
  delay(2000);
  cli();
  TCCR1B |= (1<<WGM12)|(1<<CS11);
  OCR1A = 20000;
  TIMSK1 |= (1 << OCIE1A);
  sei();
}

ISR(TIMER1_COMPA_vect){
  vel_z = g.gyro.z  - err_z;
  cur_z = vel_z*0.573;  
  z = (fabs(cur_z)>0.03)?z + (cur_z/10):z;
}

void loop() {
  mpu.getEvent(&a, &g, &temp);
  Serial.print(vel_z);
  Serial.print(" ");
  Serial.print(cur_z);
  Serial.print(" ");
  Serial.println(z);
}