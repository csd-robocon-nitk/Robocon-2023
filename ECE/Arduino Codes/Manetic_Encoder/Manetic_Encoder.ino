long counter = 0;
long t_prev, t_curr;

void setup() {
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(20, INPUT);
  pinMode(21, INPUT);
  attachInterrupt(digitalPinToInterrupt(21), count, RISING);
  digitalWrite(6, 1);
  analogWrite(7,50);
  Serial.begin(9600);
  t_prev = millis();
}

void loop() {
  t_curr = millis();
  if(t_curr-t_prev >= 1000){
    float vel = counter*0.625;
    Serial.println(vel);
    counter = 0;
    t_prev = t_curr;
  }
}

void count(){
  if(digitalRead(20)==1)
    counter ++;
  else
    counter --;
}
