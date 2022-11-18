#include <PS2X_lib.h>

#define PS2_DAT        51      
#define PS2_CMD        50  
#define PS2_SEL        53  
#define PS2_CLK        52 

int dir1 = 2, dir2 = 4, dir3 = 7, dir4 = 8;
int pwm1 = 3, pwm2 = 5, pwm3 = 6, pwm4 = 9;
PS2X ps2x;
int pwm = 50;
int deadzone = 20;

void setup() {
  Serial.begin(9600);
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
  if (abs(128-LY)>deadzone || abs(128-LX)>deadzone){
    // float lx = (LX-LY)/1.414;
    // float ly = (LX+LY)/1.414;
    float p1 = (128-LY)/128.0;       // CONTROL VARS FOR WHEELS 1,3
    float p2 = (128-LX)/128.0;       // CONTROL VARS FOR WHEELS 2,4
    p1 = (p1+p2)/1.414;
    p2 = (p1-p2)/1.414;
    // Serial.print(lx);
    // Serial.print(ly);
    Serial.print(LX);
    Serial.println(LY);
    move(round(p1*pwm),round(p2*pwm),round(p1*pwm),round(p2*pwm));
  }
  else
      move(0,0,0,0);
}

void move(int p1,int p2,int p3,int p4)
{
  // Serial.print(p1);
  // Serial.print(p2);
  // Serial.print(p3);
  // Serial.println(p4);
  analogWrite(pwm1, abs(p1));
  analogWrite(pwm2, abs(p2));
  analogWrite(pwm3, abs(p3));
  analogWrite(pwm4, abs(p4));
  if (p1>=0)
    digitalWrite(dir1,LOW);
  else
    digitalWrite(dir1,HIGH);
  if (p2>=0)
    digitalWrite(dir2,LOW);
  else
    digitalWrite(dir2,HIGH);
  if (p3>=0)
    digitalWrite(dir3,LOW);
  else
    digitalWrite(dir3,HIGH);
  if (p4>=0)
    digitalWrite(dir4,LOW);
  else
    digitalWrite(dir4,HIGH);
}