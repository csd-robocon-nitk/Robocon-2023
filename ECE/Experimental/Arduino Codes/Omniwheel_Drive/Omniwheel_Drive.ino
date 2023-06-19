#include <PS2X_lib.h>

#define PS2_DAT        51      
#define PS2_CMD        50  
#define PS2_SEL        53  
#define PS2_CLK        52 

int dir1 = 2, dir2 = 4, dir3 = 7, dir4 = 8;
int pwm1 = 3, pwm2 = 5, pwm3 = 6, pwm4 = 9;
PS2X ps2x;
int pwm = 30;

void setup() {
  Serial.begin(57600);
  pinMode(dir1,OUTPUT);
  pinMode(dir2, OUTPUT);
  pinMode(dir3, OUTPUT);
  pinMode(dir4, OUTPUT);
  pinMode(pwm1, OUTPUT);
  pinMode(pwm2, OUTPUT);
  pinMode(pwm3, OUTPUT);
  pinMode(pwm4, OUTPUT);
  pinMode(13, OUTPUT);
  analogWrite(pwm1, 0);
  analogWrite(pwm2, 0);
  analogWrite(pwm3, 0);
  analogWrite(pwm4, 0);
  digitalWrite(13, LOW);
  int error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, false, false);
  if(error == 0){
    Serial.println("Found Controller, configured successful ");
    digitalWrite(13, HIGH);   
  }
  byte type = ps2x.readType();
}

void loop() {
  ps2x.read_gamepad();  
  int LY = ps2x.Analog(PSS_LY);
  int LX = ps2x.Analog(PSS_LX);
  int RY = ps2x.Analog(PSS_RY);
  int RX = ps2x.Analog(PSS_RX);
  int i = 0;   
  if(RY<5 && RX<130 && RX>126){
    analogWrite(pwm1, pwm);
    analogWrite(pwm2, pwm);
    analogWrite(pwm3, pwm);
    analogWrite(pwm4, pwm);
    digitalWrite(dir1, LOW);
    digitalWrite(dir2, LOW);
    digitalWrite(dir3, LOW);
    digitalWrite(dir4, LOW);
    Serial.println("Forward");
    i = 1;
  }
  else if (RY>245 && RX<130 && RX>126) {
    analogWrite(pwm1, pwm);
    analogWrite(pwm2, pwm);
    analogWrite(pwm3, pwm);
    analogWrite(pwm4, pwm);
    digitalWrite(dir1, HIGH);
    digitalWrite(dir2, HIGH);
    digitalWrite(dir3, HIGH);
    digitalWrite(dir4, HIGH);
    Serial.println("Backward");
    i = 1;
  }  
  else if (RX>250 && RY<130 && RY>126){
    analogWrite(pwm1, pwm);
    analogWrite(pwm2, pwm);
    analogWrite(pwm3, pwm);
    analogWrite(pwm4, pwm);
    digitalWrite(dir1, LOW);
    digitalWrite(dir2, HIGH);
    digitalWrite(dir3, LOW);
    digitalWrite(dir4, HIGH);
    Serial.println("Right"); 
    i = 1;
  }
  else if (RX<5 && RY<130 && RY>126){
    analogWrite(pwm1, pwm);
    analogWrite(pwm2, pwm);
    analogWrite(pwm3, pwm);
    analogWrite(pwm4, pwm);
    digitalWrite(dir1, HIGH);
    digitalWrite(dir2, LOW);
    digitalWrite(dir3, HIGH);
    digitalWrite(dir4, LOW); 
    Serial.println("Left");
    i = 1;
  }
  if (LX>250 && LY<130 && LY>126){
    analogWrite(pwm1, 30);
    analogWrite(pwm2, 30);
    analogWrite(pwm3, 30);
    analogWrite(pwm4, 30);
    digitalWrite(dir1, LOW);
    digitalWrite(dir2, HIGH);
    digitalWrite(dir3, HIGH);
    digitalWrite(dir4, LOW);
    Serial.println("Clockwise");
    i = 1;    
  }
  else if (LX<5 && LY<130 && LY>126){
    analogWrite(pwm1, 30);
    analogWrite(pwm2, 30);
    analogWrite(pwm3, 30);
    analogWrite(pwm4, 30);
    digitalWrite(dir1, HIGH);
    digitalWrite(dir2, LOW);
    digitalWrite(dir3, LOW);
    digitalWrite(dir4, HIGH); 
    Serial.println("Anti-Clockwise"); 
    i = 1; 
  }
  if(i==0){
    analogWrite(pwm1, 0);
    analogWrite(pwm2, 0);
    analogWrite(pwm3, 0);
    analogWrite(pwm4, 0);
    Serial.println("Stop"); 
  }
  if(ps2x.Button(PSB_PAD_UP)) {  
      if (pwm<=65) {
        pwm = pwm + 5;
      } 
      Serial.print("Speed: ");
      Serial.println(pwm);
  }    
  if(ps2x.Button(PSB_PAD_DOWN)) {  
    if (pwm>=10) {
        pwm = pwm - 5;
    }    
    Serial.print("Speed: ");
    Serial.println(pwm);
  }
  delay(50);
}
