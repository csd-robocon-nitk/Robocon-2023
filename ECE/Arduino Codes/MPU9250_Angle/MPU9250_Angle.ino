#include "MPU9250.h"

MPU9250 mpu;

float err_y;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  delay(2000);
  
  if (!mpu.setup(0x68)) {  // change to your own address
      while (1) {
          Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
          delay(5000);
      }
  }
  Serial.print(mpu.isSleeping());
  Serial.println("Callibrating");
  for(int i = 0;i<1000;){
    if(mpu.update()){
      err_y = mpu.getYaw();
      i++;
    }
  }
}

void loop() {
    if (mpu.update()) {
        static uint32_t prev_ms = millis();
        if (millis() > prev_ms + 25) {
            print_roll_pitch_yaw();
            prev_ms = millis();
        }
    }
}

void print_roll_pitch_yaw() {
  float a = mpu.getYaw() - err_y;
  Serial.print("Yaw: ");
  Serial.println((int)a);
}
