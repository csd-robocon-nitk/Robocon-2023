#define LF 0x0A 

char msg_str[100]; 
char str_buff[7];
int idx;
String str;
int dir1 = 4, dir2 = 6, dir3 = 8;
int pwm1 = 5, pwm2 = 7, pwm3 = 9;
float mat[3][3] = {{0,-0.2,5},{0.173,0.1,5},{-0.173,0.1,5}};
float vel[3] = {0,0,0};
float pwm[3] = {0,0,0};

void setup() 
{
  Serial.begin(115200);
  Serial2.begin(115200);
  pinMode(dir1,OUTPUT);
  pinMode(dir2, OUTPUT);
  pinMode(dir3, OUTPUT);
  pinMode(pwm1, OUTPUT);
  pinMode(pwm2, OUTPUT);
  pinMode(pwm3, OUTPUT);
  analogWrite(pwm1, 0);
  analogWrite(pwm2, 0);
  analogWrite(pwm3, 0);
}

void loop() 
{
  while (Serial2.available()) 
  {
    msg_str[idx] = Serial2.read();
    if(msg_str[idx] == LF)
    {
      msg_str[idx-1] = 0;
      idx = 0;
      break;
    }
    idx++;
  }
  int i = 4;
  int j = 0;
  int var = 1;
  if(msg_str[0]=='v'&& msg_str[1]=='a'&&msg_str[2]=='l'){
    while(var<=3){
      if(msg_str[i]==' ' || msg_str[i]==0){   
        str_buff[j]=0;
        switch(var){
          case 1:
            vel[0] = atof(str_buff);
            break;
          case 2:
            vel[1] = -1*atof(str_buff);
            break;
          case 3:
            vel[2] = -1*atof(str_buff);
            break;
        }
        var++;
        j = -1;
      }
      else
      {
        str_buff[j]=msg_str[i];        
      }
      i++;
      j++;
    }  
  }
  
  Serial.print("Forward: ");
  Serial.print(vel[0]);
  Serial.print(", Normal: ");
  Serial.print(vel[1]);
  Serial.print(", Rotation: ");
  Serial.println(vel[2]);
  multiply();
  move_motor(pwm[0], pwm[1], pwm[2]);
}  

void multiply()
{
  pwm[0] = 100*(mat[0][0]*vel[0] + mat[0][1]*vel[1] + mat[0][2]*vel[2]);
  pwm[1] = 100*(mat[1][0]*vel[0] + mat[1][1]*vel[1] + mat[1][2]*vel[2]);
  pwm[2] = 100*(mat[2][0]*vel[0] + mat[2][1]*vel[1] + mat[2][2]*vel[2]);
}

void move_motor(int p1,int p2,int p3)    
{ 
  Serial.print(p1);
  Serial.print(" ");
  Serial.print(p2);
  Serial.print(" ");
  Serial.println(p3);
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
  analogWrite(pwm1, abs(p1));
  analogWrite(pwm2, abs(p2));
  analogWrite(pwm3, abs(p3));
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
}