#define LF 0x0A

char msg_str[100];
char str_buff[7];
int idx;
String str;

struct Motor {
  int PWM;
  int DIR;
  int ENCA;
  int ENCB;
  float e;
  int pwr = 0;
  float rpm_tar;
  long int t_prev,t_curr;
  int lastState,curState;
  int count_cur;
  float e_int;
  float rpm = 0.00;
};

Motor motors[4];

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
  motors[0].ENCA = 25, motors[0].ENCB = 23, motors[0].PWM = 5, motors[0].DIR = 4;
  motors[1].ENCA = 33, motors[1].ENCB = 31, motors[1].PWM = 7, motors[1].DIR = 6;
  motors[2].ENCA = 41, motors[2].ENCB = 43, motors[2].PWM = 9, motors[2].DIR = 8;
  motors[3].ENCA = 51, motors[3].ENCB = 53, motors[3].PWM = 11, motors[3].DIR = 10;
  for (int i = 0; i < 4; i++) {
    pinMode(motors[i].ENCA, INPUT);
    pinMode(motors[i].ENCB, INPUT);
    pinMode(motors[i].PWM, OUTPUT);
    pinMode(motors[i].DIR, OUTPUT);
    analogWrite(motors[i].PWM, 0);
    motors[i].t_prev = micros();
    motors[i].lastState = digitalRead(motors[i].ENCA);
  }
}

void loop() {
  while (Serial2.available()) {
    msg_str[idx] = Serial2.read();
    if (msg_str[idx] == LF) {
      msg_str[idx - 1] = 0;
      idx = 0;
      break;
    }
    idx++;
  }
  int i = 4;
  int j = 0;
  int var = 1;
  float lx = 0, ly = 0, rx = 0, ry = 0;
  if (msg_str[0] == 'v' && msg_str[1] == 'a' && msg_str[2] == 'l') {
    while (var <= 4) {
      if (msg_str[i] == ' ' || msg_str[i] == 0) {
        str_buff[j] = 0;
        switch (var) {
          case 1:
            lx = atof(str_buff);
            break;
          case 2:
            ly = atof(str_buff);
            break;
          case 3:
            rx = atof(str_buff);
            break;
          case 4:
            ry = atof(str_buff);
            break;
        }
        var++;
        j = -1;
      } else {
        str_buff[j] = msg_str[i];
      }
      i++;
      j++;
    }
  }
  motors[0].rpm_tar = ly;
  motors[1].rpm_tar = rx;
  motors[2].rpm_tar = ry;
  motors[3].rpm_tar = lx;
  for(int i = 0;i<4;i++){
    motors[i].curState = digitalRead(motors[i].ENCA);
    if (motors[i].curState != motors[i].lastState  && motors[i].curState == 1){
      if (digitalRead(motors[i].ENCB) != 1) {
        motors[i].count_cur ++;
      } else {  
        motors[i].count_cur --;
      }
    }  
    motors[i].t_curr = micros();  
    motors[i].lastState = motors[i].curState; 
  }
  for(int i = 0;i<4;i++)
  {
    if(motors[i].t_curr-motors[i].t_prev >= 500000)  
    {
      motors[i].rpm = motors[i].count_cur*60/50.0;
      motors[i].e = motors[i].rpm_tar-motors[i].rpm;
      motors[i].e_int = motors[i].e_int + motors[i].e*2;
      float p = 2*motors[i].e;
      float inte = motors[i].e_int*5.0;
      Serial.print(i); 
      Serial.print(": ");   
      Serial.print(p);
      Serial.print(" ");
      Serial.print(inte);
      Serial.print(" ");      
      motors[i].pwr = (p+inte)/40;
      if(motors[i].pwr>255)
        motors[i].pwr = 255;
      else if(motors[i].pwr<-255)
        motors[i].pwr = -255;
      if(motors[i].pwr>=0)
        digitalWrite(motors[i].DIR, 0);
      else
        digitalWrite(motors[i].DIR, 1); 
      analogWrite(motors[i].PWM, (int)fabs(motors[i].pwr));   
      motors[i].count_cur = 0;
      motors[i].t_prev = micros(); 
      Serial.print(motors[i].rpm);
      Serial.print(" ");
      else
        digitalWrite(motors[i].DIR, 1); 
      analogWrite(motors[i].PWM, (int)fabs(motors[i].pwr));   
      motors[i].count_cur = 0;
      motors[i].t_prev = micros(); 
      Serial.print(motors[i].rpm);
      Serial.print(" ");
      Serial.print(motors[i].rpm_tar);
      Serial.print(" ");
      Serial.print(motors[i].e);
      Serial.print(" ");  
      Serial.print(motors[i].pwr);
      Serial.println();           
    }    
  } 
  delay(1);
}