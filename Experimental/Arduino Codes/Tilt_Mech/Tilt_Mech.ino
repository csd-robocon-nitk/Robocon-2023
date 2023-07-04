//Code for tilt mechanism automation. The program moves to tile mechanism to a desired angle.

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

float x = 0;
float y = 0;
float z = 0;

// Target angle
float tar_ang=60;
float e_int=0;
float err_x,err_y,err_z;
int pwm=0;
long t_prev, t_curr;
long t_prev_mpu, t_curr_mpu;
float del_T=100;

// Defining pins for motor driver
#define DirPin 10
#define PwmPin 11

Adafruit_MPU6050 mpu;

void setup(void) 
{
  Serial.begin(115200);
  pinMode(DirPin, OUTPUT);
  pinMode(PwmPin, OUTPUT);
  digitalWrite(DirPin, 0);

  // Setting up MPU6050
  Wire.begin();
  Wire.beginTransmission(0x68);
  Wire.write(0x6B); 
  Wire.write(0x00);     
  Wire.endTransmission(true);
  Serial.println("Adafruit MPU6050 test!");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("");
  delay(1000);

  // Calibrating MPU6050
  Serial.println("Calibrating....Do not move mpu6050");
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  err_x = g.gyro.x;
  err_y = g.gyro.y;
  err_z = g.gyro.z;
  Serial.println("Done");
  t_prev = millis();
  t_prev_mpu = millis();
}

void loop() 
{ 
  // Calculate angle every 10 milliseconds
	t_curr_mpu = millis();
	if(t_curr_mpu-t_prev_mpu >= 10)
  {
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
		
		Serial.print("X: ");
		Serial.print(round(x));
		Serial.print(", Y: ");
		Serial.print(round(y));
		Serial.print(", Z: ");
		Serial.println(round(z));
		t_prev = millis();
  }

  // PI controller for linear actuator
  t_curr = millis();
  if(t_curr-t_prev >= del_T)
  {
	  int e=tar_ang-z;
	  e_int=e_int+e*1;
	  float p=5*e;
	  float i=3*e_int;
	  pwm=(p+i)/40;
	  if (pwm>255)
	    pwm=255;
	  else if (pwm<-255)
	    pwm=-255;
	  if (pwm>=0)
	    digitalWrite(DirPin, 0);
	  else
	    digitalWrite(DirPin,1);
	  analogWrite(PwmPin,(int)fabs(pwm));
	  t_prev = millis();
  }
}
