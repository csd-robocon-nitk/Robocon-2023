#define LF 0x0A 

char msg_str[100]; 
char str_buff[7];
int idx;
String str;
int dir1 = 4, dir2 = 6, dir3 = 8, dir4 = 10;
int pwm1 = 5, pwm2 = 7, pwm3 = 9, pwm4 = 11;
float p1 = 0, p2 = 0, p3 = 0, p4 = 0;

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
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
  while (Serial2.available()) {
    msg_str[idx] = Serial2.read();
    if(msg_str[idx] == LF){
      msg_str[idx-1] = 0;
      idx = 0;
      break;
    }
    idx++;
  }
  //Serial.println(msg_str);
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
  if(lx >= 0 && lx<p1){
    lx = max(lx,p1-3);
  } 
  else{
    lx = min(lx,p1+3);  
  }
  if(ly<p2){
    ly = max(ly,p2-3);
  } 
  else{
    ly = min(ly,p2+3);  
  }
  if(rx<p3){
    rx = max(rx,p3-3);
  }
  else{
    rx = min(rx,p3+3);  
  } 
  if(ry<p4){
    ry = max(ry,p4-3);
  }   
  else{
    ry = min(ry,p4+3);  
  } 
  Serial.print("Motor1: ");
  Serial.print(ly);
  Serial.print(", Motor2: ");
  Serial.print(rx);
  Serial.print(", Motor3: ");
  Serial.print(ry);
  Serial.print(", Motor4: ");
  Serial.println(lx);   
  move(round(ly),round(rx),round(ry),round(lx));
  p2 = ly;
  p1 = lx;  
  p3 = rx;
  p4 = ry;
}  

void move(int p1,int p2,int p3,int p4)    
{ 
  if(p1>40)
    p1 = 40;
  else if (p1<-40)
    p1 = -40; 
  if(p2>40)
    p2 = 40;
  else if (p2<-40)
    p2 = -40;
  if(p3>40)
    p3 = 40;
  else if (p3<-40)
    p3 = -40;
  if(p4>40)
    p4 = 40;
  else if (p4<-40)
    p4 = -40;
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
