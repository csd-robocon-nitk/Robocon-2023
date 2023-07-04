// Program to read the velocity of the motor using magnetic encoder

// Pins for the motor
#define DIR 6
#define PWM 7
#define ENCA 20
#define ENCB 21

// Defining variables
long counter = 0;
long t_prev, t_curr;

void setup() {
  Serial.begin(115200);

  pinMode(DIR,OUTPUT);
  pinMode(PWM,OUTPUT);
  pinMode(ENCA, INPUT);
  pinMode(ENCB, INPUT);

  // Interrupt for ENCA or ENCB
  attachInterrupt(digitalPinToInterrupt(ENCB), count, RISING);

  // Run motor at a specific speed
  digitalWrite(DIR, 1);
  analogWrite(PWM,50);

  // Record the current time
  t_prev = millis();
}

void loop() {
  // Calculate the velocity every 1 second
  t_curr = millis();
  if(t_curr-t_prev >= 1000){
    float vel = counter*0.625;
    Serial.println(vel);
    counter = 0;
    t_prev = t_curr;
  }
}

// Interrupt handler to count the number of pulses
void count(){
  if(digitalRead(ENCA)==1)
    counter ++;
  else
    counter --;
}
