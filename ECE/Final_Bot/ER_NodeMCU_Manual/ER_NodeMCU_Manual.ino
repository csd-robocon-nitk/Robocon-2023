#define LF 0x0A

char msg_str[100];
char str_buff[7];
int idx;

int gantry_pwm = 5;
int gantry_dir = 4;
int gantry_lmt_r = 2;
int gantry_lmt_l = 13;

int lift_pwm = 12;
int lift_dir = 14;
int lift_lmt_u = 10;
int lift_lmt_d = 9;

int lift_sts;
int reload_trgr, prev_reload_trgr;



void setup() {
  // put your setup code here, to run once:

  pinMode(gantry_pwm, OUTPUT);
  pinMode(gantry_dir, OUTPUT);
  pinMode(lift_dir, OUTPUT);
  pinMode(lift_pwm, OUTPUT);
  pinMode(gantry_lmt_r, INPUT_PULLUP);
  pinMode(gantry_lmt_l, INPUT_PULLUP);
  pinMode(lift_lmt_u, INPUT_PULLUP);
  pinMode(lift_lmt_d, INPUT_PULLUP);

  Serial.begin(115200);
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
  if (reload_trgr == 1 && prev_reload_trgr != reload_trgr) {
    while (digitalRead(gantry_lmt_l)) {
      digitalWrite(gantry_dir, 0);
      digitalWrite(gantry_pwm, 1);
      delay(1);
    }
    digitalWrite(gantry_pwm, 0);
    while (digitalRead(lift_lmt_u)) {
      digitalWrite(lift_dir, 0);
      digitalWrite(lift_pwm, 1);
      delay(1);
    }
    digitalWrite(lift_pwm, 0);
    while (digitalRead(gantry_lmt_r)) {
      digitalWrite(gantry_dir, 1);
      digitalWrite(gantry_pwm, 1);
      delay(1);
    }
    digitalWrite(gantry_pwm, 0);
    while (digitalRead(gantry_lmt_l)) {
      digitalWrite(gantry_dir, 0);
      digitalWrite(gantry_pwm, 1);
      delay(1);
    }
    digitalWrite(gantry_pwm, 0);
    while (digitalRead(lift_lmt_u)) {
      digitalWrite(lift_dir, 0);
      digitalWrite(lift_pwm, 1);
      delay(1);
    }
    digitalWrite(lift_pwm, 0);
    delay(300);
  }
  prev_reload_trgr = reload_trgr;
  if(lift_sts != 0)
  {
    if(lift_sts == 1)
      digitalWrite(lift_dir, 0);
    else
      digitalWrite(lift_dir, 1);
    digitalWrite(lift_pwm, 1);
  }
  else
  {
    digitalWrite(lift_pwm, 0);
  }
}