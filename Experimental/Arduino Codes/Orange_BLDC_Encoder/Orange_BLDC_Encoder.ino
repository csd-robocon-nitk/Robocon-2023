// Code to test encoder in orange bldc motor

#define ENC 2
int counter = 0;
float t_prev,t_curr;

void setup() {
  Serial.begin(9600);
  pinMode(ENC,INPUT);

  // Attach interrupt to count the number of pulses
  attachInterrupt(digitalPinToInterrupt(ENC), count, RISING);
  t_prev = millis();
}

void loop() {
  // Calculate the velocity every 1 second
  t_curr = millis();
  if(t_curr-t_prev >= 1000){
    float vel = counter * 2;
    Serial.println(vel);
    t_prev = millis();
    counter = 0;
  }
}

// Interrupt handler to count the number of pulses
void count(){
  counter += 1;
}