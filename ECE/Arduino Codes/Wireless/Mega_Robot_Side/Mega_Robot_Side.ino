#define LF 0x0A 

char msg_str[100]; 
char str_buff[7];
int idx;
String str;
int dir1 = 2, dir2 = 4, dir3 = 7, dir4 = 8;
int pwm1 = 3, pwm2 = 5, pwm3 = 6, pwm4 = 9;

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
  analogWrite(pwm1, 0);
  analogWrite(pwm2, 0);
  analogWrite(pwm3, 0);
  analogWrite(pwm4, 0);
  digitalWrite(13, LOW);
}

void loop() {
  while (Serial.available()) {
    msg_str[idx] = Serial.read();
    if(msg_str[idx] == LF) {
      msg_str[idx-1] = 0;
      idx = 0;
      break;
    }
    idx++;
  }
  
  int i = 4;
  int j = 0;
  int var =1;
  float lx =0, ly=0, rx=0, ry=0;
  if(msg_str[0]=='v'&& msg_str[1]=='a'&&msg_str[2]=='l'){
    while(var<=4){
      if(msg_str[i]==' ' || msg_str[i]==0){   
        str_buff[j]=0;
        switch(var){
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
      }
      else{
        str_buff[j]=msg_str[i];        
      }
      i++;
      j++;
    }  
  }
  Serial.print("motor1:");
  Serial.print(lx);
  Serial.print(" motor2:");
  Serial.print(ly);
  Serial.print(" motor3:");
  Serial.print(rx);
  Serial.print(" motor4:");
  Serial.println(ry);
}  

void move(int p1,int p2,int p3,int p4)
{
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