#include <PS2X_lib.h>

//PS2 controller pin connections
#define PS2_DAT 11
#define PS2_CMD 12
#define PS2_SEL 10
#define PS2_CLK 13

// Pin numbers of other buttons.......Change these
#define plate_up A7
#define plate_down A6
#define rotate_cw 3
#define rotate_ccw 5
#define lock_x 6
#define lock_y 7
#define type1 A5
#define type2 A4
#define type3 8
#define type4 4
#define type5 2
#define adj_pos 9

// Pins of led indicators...Change these too
#define lockx_in A1
#define locky_in A2
#define speed_in A3

PS2X ps2x;
String msg;

int lockx = 0, locky = 0, speed = 0;
int lockx_prev = 0, locky_prev = 0;

void setup() {
  Serial.begin(115200);
  // Configure the PS2 controller to work with Arduino Nano
  int error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, false, false);
  // Configure all other buttons and leds
  pinMode(rotate_cw, INPUT_PULLUP);
  pinMode(rotate_ccw, INPUT_PULLUP);
  pinMode(lock_x, INPUT_PULLUP);
  pinMode(lock_y, INPUT_PULLUP);
  pinMode(type1, INPUT_PULLUP);
  pinMode(type2, INPUT_PULLUP);
  pinMode(type3, INPUT_PULLUP);
  pinMode(type4, INPUT_PULLUP);
  pinMode(type5, INPUT_PULLUP);
  pinMode(adj_pos, INPUT_PULLUP);
  pinMode(lockx_in, OUTPUT);
  pinMode(locky_in, OUTPUT);
  pinMode(speed_in, OUTPUT);
  digitalWrite(lockx_in, LOW);
  digitalWrite(locky_in, LOW);
  digitalWrite(speed_in, LOW);
}

void loop() {
  ps2x.read_gamepad();

  // Handling reset buttons first
  int Rst_Nano = ps2x.Button(PSB_CIRCLE);
  int Rst_MPU = ps2x.Button(PSB_TRIANGLE);
  int Rst_Mega = ps2x.Button(PSB_CROSS);
  if (Rst_Mega)
    msg = "rst_mega";
  else if (Rst_Nano)
    msg = "rst_nano";
  else if (Rst_MPU)
    msg = "rst_MPU";
  else if (digitalRead(adj_pos) == 0)
    // Adjusting position command
    msg = "adj";
  else if (digitalRead(type1) == 0)
    // Aim at type 1 pole
    msg = "type1";
  else if (digitalRead(type2) == 0)
    // Aim at type 2 pole
    msg = "type2";
  else if (digitalRead(type3) == 0)
    // Aim at type 3 pole
    msg = "type3";
  else if (digitalRead(type4) == 0)
    // Aim at type 4 pole
    msg = "type4";
  else if (digitalRead(type5) == 0)
    // Aim at type 5 pole
    msg = "type5";
  else if (digitalRead(rotate_cw) == 0)
    // Rotate the robot clockwise 90 degrees
    msg = "cw";
  else if (digitalRead(rotate_ccw) == 0)
    // Rotate the robot counter-clockwise 90 degrees
    msg = "ccw";
  else {
    int Reload = ps2x.Button(PSB_L1);
    int Shoot = ps2x.Button(PSB_R1);

    if (Reload)
      msg = "rel";
    else if (Shoot)
      msg = "sht";
    else {
      //Handling rest other buttons on PS2 controller
      int LY = ps2x.Analog(PSS_LY);
      int LX = ps2x.Analog(PSS_LX);
      int RX = ps2x.Analog(PSS_RX);
      int Speed = ps2x.ButtonReleased(PSB_SQUARE);
      int Angle_Up = ps2x.Button(PSB_PAD_UP);
      int Angle_Down = ps2x.Button(PSB_PAD_DOWN);
      int Pull = ps2x.Button(PSB_PAD_LEFT);
      int Push = ps2x.Button(PSB_PAD_RIGHT);

      // Hangling pick buttons
      int pick;
      if (analogRead(plate_up) == 0)
        pick = 1;
      else if (analogRead(plate_down) == 0)
        pick = -1;
      else
        pick = 0;

      // Handling lock buttons
      if (digitalRead(lock_x) == 0 && lockx_prev == 1) {
        lockx = !lockx;
        lockx_prev = 0;
      } else if (digitalRead(lock_x) == 1 && lockx_prev == 0) {
        lockx_prev = 1;
      }
      if (digitalRead(lock_y) == 0 && locky_prev == 1) {
        locky = !locky;
        locky_prev = 0;
      } else if (digitalRead(lock_y) == 1 && locky_prev == 0) {
        locky_prev = 1;
      }

      //Calculate velocities
      float vel_y = ((128 - LY) * 100 / 128.0);  //- 3.91;            //Uncomment to remove offset if any
      float vel_x = ((LX - 128) * 100 / 128.0);  //+ 3.91;
      float vel_w = ((RX - 128) * 100 / 128.0);  //+ 3.91;

      //Values to move stepper and tilt motors
      int tilt, stretch;
      if (Angle_Up)
        tilt = 1;
      else if (Angle_Down)
        tilt = -1;
      else
        tilt = 0;
      if (Pull)
        stretch = 1;
      else if (Push)
        stretch = -1;
      else
        stretch = 0;

      // Handling speed button
      if (Speed == 1) {
        speed = !speed;
      } 

      // Turn on indicators accordingly
      if (speed)
        digitalWrite(speed_in, HIGH);
      else
        digitalWrite(speed_in, LOW);
      if (lockx)
        digitalWrite(lockx_in, HIGH);
      else
        digitalWrite(lockx_in, LOW);
      if (locky)
        digitalWrite(locky_in, HIGH);
      else
        digitalWrite(locky_in, LOW);

      // val <velx> <vely> <velw> <pick> <stretch> <tilt> <lockx> <locky> <speed>
      msg = String("val") + String(" ") + String(vel_y) + String(" ") + String(vel_x) + String(" ") + String(vel_w) + " " + String(pick) + " " + String(stretch) + String(" ") + String(tilt) + " " + String(lockx) + " " + String(locky) + " " + String(speed);
    }
  }

  // Send this message to ESP07
  Serial.println(msg);
}