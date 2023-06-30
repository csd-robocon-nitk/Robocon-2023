//Program to control latch of shooting mechanism using a button

#include <Servo.h>

#define Button_Pin 11
#define Servo_Pin 10

Servo myservo; 

// Variable to store servo angle
int pos = 0; 

// For button debouncing
int buttonState=1;
int state=0;
int prevState=1;

long int t1;

void setup() {
  myservo.attach(Servo_Pin);  
  pinMode(Button_Pin,INPUT_PULLUP);

  // Set angle of servo to initial angle
  myservo.write(pos);
}

void loop() {
  buttonState=digitalRead(Button_Pin);
  if(buttonState){
    prevState=1;
  }
  if(buttonState==LOW && buttonState!=prevState){
    prevState=buttonState;
    // Toggle between 0 and 90 degrees
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
