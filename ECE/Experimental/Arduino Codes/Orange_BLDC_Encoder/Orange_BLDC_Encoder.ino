int counter = 0;
float t_prev,t_curr;

void setup() {
  pinMode(2,INPUT);
  attachInterrupt(digitalPinToInterrupt(2), count, RISING);
  Serial.begin(9600);
}

void loop() {
  t_curr = millis();
  if(t_curr-t_prev >= 1000){
    float vel = counter * 2;
    Serial.println(vel);
    t_prev = millis();
    counter = 0;
  }
}

void count(){
  counter += 1;
}