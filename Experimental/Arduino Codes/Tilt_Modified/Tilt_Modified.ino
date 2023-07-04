//Code for tilt mechanism automation. The program moves to tile mechanism to a desired angle.
// Note: This uses accelerometer values instead of gyro value from mpu6050

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// Target angle
float tar_ang=60;
float pwm=0;
long t_prev, t_curr;
float del_T=100;

// Pins for linear actuator
#define DirPin 4
#define PwmPin 5

Adafruit_MPU6050 mpu;

void setup(void)
{
  Serial.begin(115200);

  // Setting up linear actuator
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
  if (!mpu.begin()) 
  {
    Serial.println("Failed to find MPU6050 chip");
    while (1) 
    {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("");
  delay(1000);

  // Calibrating MPU6050
  Serial.println("Calibrating....Do not move mpu6050");
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  t_prev = millis();
}

void loop()
{
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Using acceleration components to find angle of inclination
  float AccX=a.acceleration.x;
  float AccY=a.acceleration.y;
  float AccZ=a.acceleration.z;
  float ang_rad=atan(AccY/AccZ);
  float ang_deg=ang_rad*(180)/(3.141592653);

  /* For debuggin purposes
  Serial.print("Acceleration X: ");
  Serial.print(AccX);
  Serial.print(", Y: ");

  Serial.print(AccY);
  Serial.print(", Z: ");
  Serial.print(AccZ);
  Serial.println(" m/s^2");
  */
  Serial.print("Angle: ");
  Serial.println(ang_deg);

  // PI controller for linear actuator
  t_curr = millis();
  if(t_curr-t_prev >= del_T)
  {
	  int e=tar_ang-ang_deg;
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
