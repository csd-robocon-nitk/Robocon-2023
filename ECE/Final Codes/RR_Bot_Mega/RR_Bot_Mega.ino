#define LF 0x0A

float z = 0;

char msg_str[100];
char str_buff[7];
int idx;

float conv_matrix[3][3] = { { 0, -1.3334, 0.8 }, { 1.1547, 0.6667, 0.8 }, { -1.1547, 0.6667, 0.8 } };

// Variable to determine locking x and y direction motion
int lockx = 0, locky = 0;

// Motor class with all motor properties
class Motor {
public:
  int PWM;
  int DIR;
  int ENCA;
  int ENCB;
  int count;
  float e = 0;
  float e_prev = 0;
  float pwr = 0;
  short int dir = 0;
  float rps_tar;
  long int t_prev, t_curr;
  float e_int = 0;
  float e_d = 0;
  float rps = 0.00;
  float kp = 0;
  float ki = 0;

  Motor() {}
  Motor(int pwm, int dir) {
    PWM = pwm;
    DIR = dir;
    count = 0;
    pinMode(PWM, OUTPUT);
    pinMode(DIR, OUTPUT);
    analogWrite(PWM, LOW);
    digitalWrite(DIR, LOW);
  }
  Motor(int pwm, int dir, int enca, int encb, float kp_value, float ki_value) {
    PWM = pwm;
    DIR = dir;
    ENCA = enca;
    ENCB = encb;
    kp = kp_value;
    ki = ki_value;
    count = 0;
    pinMode(ENCA, INPUT);
    pinMode(ENCB, INPUT);
    pinMode(PWM, OUTPUT);
    pinMode(DIR, OUTPUT);
    analogWrite(PWM, 0);
    digitalWrite(DIR, 0);
    t_prev = millis();
  }
};

// Initializing three instances of the motors
Motor motors[3];

// Local and Global frame velocities
float vel[3] = { 0, 0, 0 };
float glb_vel[3] = { 0, 0, 0 };

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);

  // Defining motor parameters
  motors[0] = Motor(4, 22, 2, 34, 20, 18);   //Motor(pwm_pin, dir_pin, enc_a_pin, enc_b_pin, kp_value, ki_value)
  motors[1] = Motor(5, 23, 3, 35, 20, 18);
  motors[2] = Motor(6, 24, 18, 36, 20, 18);

  // Setting up interrupts for encoders
  attachInterrupt(digitalPinToInterrupt(motors[0].ENCA), update_count_0, RISING);
  attachInterrupt(digitalPinToInterrupt(motors[1].ENCA), update_count_1, RISING);
  attachInterrupt(digitalPinToInterrupt(motors[2].ENCA), update_count_2, RISING);
}

void loop() {
  // Retrieving data from esp07
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

  //splitting the data
  if (msg_str[0] == 'v' && msg_str[1] == 'a' && msg_str[2] == 'l') {
    while (var <= 9) {
      if (msg_str[i] == ' ' || msg_str[i] == 0) {
        str_buff[j] = 0;
        switch (var) {
          case 1:
            glb_vel[0] = atof(str_buff);
            break;
          case 2:
            glb_vel[1] = atof(str_buff);
            break;
          case 3:
            glb_vel[2] = -1 * atof(str_buff);
            break;
          case 7:
            lockx = atoi(str_buff);
            break;
          case 8:
            locky = atoi(str_buff);
            break;
          case 9:
            z = atof(str_buff);
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

  //Lock x and y direction motion logic
  if(lockx == 1)
    glb_vel[1] = 0;
  if(locky == 1)
    glb_vel[0] = 0;
    
  multiply();

  // Printing motor rpm for debugging
  Serial.print(motors[0].rps);
  Serial.print(" ");
  Serial.print(motors[1].rps);
  Serial.print(" ");
  Serial.print(motors[2].rps);
  Serial.print(" ");
  Serial.println(z);
  move_motor(&motors[0]);
  move_motor(&motors[1]);
  move_motor(&motors[2]);
}

// Function to convert bot velocity to velocity of each wheel
void multiply() {
  float angle = z;
  if (angle >= 360)
    angle = angle - 360;
  if (angle <= -360)
    angle = angle + 360;
  angle = angle * PI / 180;
  vel[0] = glb_vel[0] * cos(angle) - glb_vel[1] * sin(angle);
  vel[1] = glb_vel[0] * sin(angle) + glb_vel[1] * cos(angle);
  vel[2] = glb_vel[2];
  motors[0].rps_tar = (conv_matrix[0][0] * vel[0] + conv_matrix[0][1] * vel[1] + conv_matrix[0][2] * vel[2])/25;
  motors[1].rps_tar = (conv_matrix[1][0] * vel[0] + conv_matrix[1][1] * vel[1] + conv_matrix[1][2] * vel[2])/25;
  motors[2].rps_tar = (conv_matrix[2][0] * vel[0] + conv_matrix[2][1] * vel[1] + conv_matrix[2][2] * vel[2])/25;
}

//Function to move the motor
void move_motor(Motor* m) {
  m->t_curr = millis();
  if (m->t_curr - m->t_prev >= 200) {
    m->rps = m->count / 6;
    if(m->rps_tar < 0)
      m->rps = -1*m->rps;
    m->e = m->rps_tar - m->rps;
    m->e_int = m->e_int + (m->e * 0.2);
    m->pwr = m->kp * m->e + m->ki * m->e_int;
    m->count = 0;
    m->t_prev = millis();
  }
  if(m->rps_tar == 0)
  {
    m->e = 0;
    m->e_int = 0;
    m->pwr = 0;
  }
  if (m->rps_tar > 0) {
    m->dir = 1;
  } else if(m->rps_tar < 0){
    m->dir = 0;
  } else {
    m->dir = !m->dir;
  }
  digitalWrite(m->DIR, m->dir);
  analogWrite(m->PWM, (int)fabs(m->pwr));
}

// Interrupt routines
void update_count_0() {
  motors[0].count++;
}

void update_count_1() {
  motors[1].count++;
}

void update_count_2() {
  motors[2].count++;
}