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
int ULTRASensor = 7; 
int INPUT_DISTANCE = 50;
int CONSTANT_OMEGA = 0
int const_error=2

void setup() 
{ pinMode (ULTRASensor, INPUT);
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

 float pid_distance(distance){
   errorD = distance - INPUT_DISTANCE;
   integral = integral + errorD*dt;
   derivative = (errorD - previousErrorD)/dt;
   output = kp*errorD + ki*integral + kd*derivative;
   previousErrorD  = errorD;

  return output;
  }

 float pid_omega(omega):{
   errorD = omega - CONSTANT_OMEGA;
   integral = integral + errorD*dt;
   derivative = (errorD - previousErrorD)/dt;
   output = kp*errorD + ki*integral + kd*derivative;
   previousErrorD  = errorD;

  return output;
  }


void loop()
 {
  //Define a variables for read the ULTRASensor   
  int dist_read = digitalRead (ULTRASensor); 
  pidDist = ceil(pid_distance(dist_read));
  if (dist_read - INPUT_DISTANCE> const_error){
    vel[1] = vel[1] + pidDist;
    }
  else{
    vel[1] = vel[1] - pidDist;
    }
  multiply();
  move_motor(pwm[0], pwm[1], pwm[2]); 
 }
