//Program to control latch of shooting mechanism

#include <Servo.h>
Servo myservo;  
int pos = 11;    // variable to store the servo position
int buttonState=1;
int buttonPin=11;
int state=0;
int prevState=1;
long int t1;
void setup() {
  myservo.attach(10);  // attaches the servo on pin 10 to the servo object
  pinMode(buttonPin,INPUT_PULLUP);
  myservo.write(pos);
}

void loop() {
  buttonState=digitalRead(buttonPin);
  if(buttonState){
    prevState=1;
  }
  if(buttonState==LOW && buttonState!=prevState){
    prevState=buttonState;
    if(state==90){
      myservo.write(0);
      state=0;
    }
    else{
      myservo.write(90);
      state=90;
    }
    delay(500);
  }
}
