#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

// Select a Timer Clock
#define USING_TIM_DIV1                false           // for shortest and most accurate timer
#define USING_TIM_DIV16               false           // for medium time and medium accurate timer
#define USING_TIM_DIV256              true            // for longest timer but least accurate. Default

#include "ESP8266TimerInterrupt.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

float z = 0;
sensors_event_t a, g, temp;
float vel_z;
float cur_z;

float err_z;

Adafruit_MPU6050 mpu;

#define TIMER_INTERVAL_MS        500   //1000

ESP8266Timer ITimer;

void TimerHandler()
{
  vel_z = g.gyro.z - err_z;
  cur_z = vel_z*0.573;
  z = (fabs(cur_z)>0.03)?z + (cur_z):z;
}

void setup()
{
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
  err_z = g.gyro.z;
  Serial.println("Done");
  Serial.println("");
  delay(2000);
  pinMode(LED_BUILTIN, OUTPUT);
  if (ITimer.attachInterruptInterval(TIMER_INTERVAL_MS * 20, TimerHandler))
  {
    Serial.print(F("Starting  ITimer OK, millis() = ")); Serial.println(millis());
  }
  else
    Serial.println(F("Can't set ITimer. Select another freq. or timer"));
}

#define CHECK_INTERVAL_MS     10000L
#define CHANGE_INTERVAL_MS    20000L

void loop()
{
  mpu.getEvent(&a, &g, &temp);
  Serial.println(z);
}
