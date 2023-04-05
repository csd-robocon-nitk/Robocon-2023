#define LF 0x0A

char msg_str[100];
char str_buff[7];
int idx;

class Motor {
public:
  int PWM;
  int DIR;

  Motor() {}
  Motor(int pwm, int dir) {
    PWM = pwm;
    DIR = dir;
    pinMode(PWM, OUTPUT);
    pinMode(DIR, OUTPUT);
    digitalWrite(PWM, LOW);
    digitalWrite(DIR, LOW);
  }
};

Motor lift;
Motor gantry;

int gantry_pwm = 10;
int gantry_dir = 9;
int gantry_lmt_r = 4;
int gantry_lmt_l = 0;

int lift_pwm = 12;
int lift_dir = 14;
int lift_lmt_u = 5;
int lift_lmt_d = 2;

int lift_sts;
int reload_trgr;



void setup() {
  // put your setup code here, to run once:

  lift = Motor(lift_pwm, lift_dir);
  gantry = Motor(gantry_pwm, gantry_dir);

  pinMode(16,OUTPUT);

  pinMode(gantry_lmt_r, INPUT_PULLUP);
  pinMode(gantry_lmt_l, INPUT_PULLUP);
  pinMode(lift_lmt_u, INPUT_PULLUP);
  pinMode(lift_lmt_d, INPUT_PULLUP);

  digitalWrite(16,LOW);

}

void loop() {
  // put your main code here, to run repeatedly:
  while (Serial.available()) {
    msg_str[idx] = Serial.read();
    if (msg_str[idx] == LF) {
      msg_str[idx - 1] = 0;
      idx = 0;
      break;
    }
    idx++;
  }
  //Serial.println(msg_str);
  int i = 4;
  int j = 0;
  int var = 1;

  if (msg_str[0] == 'v' && msg_str[1] == 'a' && msg_str[2] == 'l') {
    while (var <= 9) {
      if (msg_str[i] == ' ' || msg_str[i] == 0) {
        str_buff[j] = 0;
        switch (var) {
          case 4:
            lift_sts = atoi(str_buff);
            break;
          case 6:
            reload_trgr = atoi(str_buff);
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

  pick(&lift, lift_sts);
}

void pick(Motor *m, int sts) {
  int pwm;
  if (sts == 0) pwm = 0;
  else if (sts == 1) {
    pwm = 1;
    digitalWrite(m->DIR, 0);
  } else if (sts == -1) {
    pwm = 1;
    digitalWrite(m->DIR, 1);
  }
  digitalWrite(m->PWM, pwm);
}
