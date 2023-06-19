#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

float tar_ang=60;
float pwm=0;
long t_prev, t_curr;
float del_T=100;

#define DirPin 4
#define PwmPin 5

Adafruit_MPU6050 mpu;

void setup(void)
{
  Serial.begin(115200);
  pinMode(DirPin, OUTPUT);
  pinMode(PwmPin, OUTPUT);
  digitalWrite(DirPin, 0);
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
  Serial.println("Calibrating....Do not move mpu6050");
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  t_prev = millis();
}

void loop()
{
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float AccX=a.acceleration.x;
  float AccY=a.acceleration.y;
  float AccZ=a.acceleration.z;
  float ang_rad=atan(AccY/AccZ);
  float ang_deg=ang_rad*(180)/(3.141592653);

  Serial.print("Acceleration X: ");
  Serial.print(AccX);
  Serial.print(", Y: ");

  Serial.print(AccY);
  Serial.print(", Z: ");
  Serial.print(AccZ);
  Serial.println(" m/s^2");
  Serial.print("Angle: ");
  Serial.println(ang_deg);

  if (tar_ang>ang_deg){
    pwm = ((tar_ang - ang_deg)/tar_ang)*255;
  }
  if (tar_ang<ang_deg){
    pwm = ((tar_ang - ang_deg)/tar_ang)*255;
  }
  if (tar_ang = ang_deg)
  {
    pwm = 255;
  }
