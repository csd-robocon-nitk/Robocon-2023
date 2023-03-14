#define LF 0x0A

char msg_str[100];
char str_buff[7];
int idx;
String str;

class Motor {
public:
  int PWM;
  int DIR;
  int ENC;
  int count;
  float e;
  float pwr = 0;
  short int dir = 0;
  float rpm_tar;
  long int t_prev, t_curr;
  float e_int;
  float rpm = 0.00;

  Motor() {}
  Motor(int pwm, int dir, int enc) {
    PWM = pwm;
    DIR = dir;
    ENC = enc;
    count = 0;
    pinMode(ENC, INPUT);
    pinMode(PWM, OUTPUT);
    pinMode(DIR, OUTPUT);
    analogWrite(PWM, (int)fabs(pwr));
    digitalWrite(DIR, dir);
    t_prev = millis();
  }
};

Motor motors[3];

float mat[3][3] = { { 0, -1, 0.18 }, { 1.1547, 2, 0.18 }, { -1.1547, 2, 0.18 } };

float vel[3] = { 0, 0, 0 };

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);

  motors[0] = Motor(5, 4, 2);
  motors[1] = Motor(7, 6, 3);
  motors[2] = Motor(9, 8, 18);

  attachInterrupt(digitalPinToInterrupt(motors[0].ENC), motors[0].count++, RISING);
  attachInterrupt(digitalPinToInterrupt(motors[1].ENC), motors[1].count++, RISING);
  attachInterrupt(digitalPinToInterrupt(motors[2].ENC), motors[2].count++, RISING);
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
  if (msg_str[0] == 'v' && msg_str[1] == 'a' && msg_str[2] == 'l') {
    while (var <= 3) {
      if (msg_str[i] == ' ' || msg_str[i] == 0) {
        str_buff[j] = 0;
        switch (var) {
          case 1:
            vel[0] = atof(str_buff);
            break;
          case 2:
            vel[1] = atof(str_buff);
            break;
          case 3:
            vel[2] = atof(str_buff);
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
  
  multiply();
  Serial.print(motors[0].rpm_tar);
  Serial.print(" ");
  Serial.print(motors[1].rpm_tar);
  Serial.print(" ");
  Serial.println(motors[2].rpm_tar);
  move_motor(motors[0]);
  move_motor(motors[1]);
  move_motor(motors[2]);
  //Serial.println();
}

void multiply() {
  motors[0].rpm_tar = (mat[0][0] * vel[0] + mat[0][1] * vel[1] + mat[0][2] * vel[2]);
  motors[1].rpm_tar = (mat[1][0] * vel[0] + mat[1][1] * vel[1] + mat[1][2] * vel[2]);
  motors[2].rpm_tar = (mat[2][0] * vel[0] + mat[2][1] * vel[1] + mat[2][2] * vel[2]);
}

void move_motor(Motor m) {
  m.t_curr = millis();
  if (m.t_curr - m.t_prev >= 100) {
    m.rpm = m.count * 20;
    m.e = m.rpm_tar - m.rpm;
    m.e_int = m.e_int + (m.e * 100);
    m.pwr = m.e + m.e_int;
    m.count = 0;
    m.t_prev = millis();
  }
  //Serial.print(m.rpm);
  if (m.rpm_tar >= 0) {
    m.dir = 0;
  } else {
    m.dir = 1;
  }
  digitalWrite(m.DIR, m.dir);
  analogWrite(m.PWM, (int)fabs(m.pwr));
}