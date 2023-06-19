#define encA 51
#define encB 53
#define pwm_pin 11
#define dir 10

int count_cur = 0, count_prev = 0;
float e_int = 0;
int pwm = 10,rpm_tar = - 80;
float rpm = 0.00;
int currentState;
int lastState;
long int t_prev, t_curr;

void setup() {
	pinMode(encA,INPUT);
	pinMode(encB,INPUT);
  pinMode(pwm_pin, OUTPUT);
  pinMode(dir, OUTPUT);
	Serial.begin(9600);
  t_curr = micros();
	lastState = digitalRead(encA);
  t_prev = micros();
}

void loop() {
	currentState = digitalRead(encA);
	if (currentState != lastState  && currentState == 1){
		if (digitalRead(encB) != 1) {
			count_cur ++;
		} else {  
			count_cur --;
    }
	}
  t_curr = micros();
  if(t_curr-t_prev >= 300000)
  {
    rpm = count_cur*60/30.00;
    int e = rpm_tar - rpm;
    Serial.print(e);
    Serial.print(" ");
    e_int = e_int + e*3;
    float p = 4*e;
    float i = 2*e_int;
    pwm = (p+i)/40;
    if(pwm>255)
      pwm = 255;
    else if(pwm<-255)
      pwm = -255;
    if(pwm>=0)
      digitalWrite(dir, 0);
    else
      digitalWrite(dir, 1);
    analogWrite(pwm_pin, (int)fabs(pwm));
    Serial.print(pwm);
    Serial.print(" ");
    Serial.println(rpm);
    count_cur = 0;
    t_prev = micros();
  } 
	lastState = currentState;
	delay(1);
}
