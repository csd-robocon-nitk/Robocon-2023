#include <PS2X_lib.h>

//PS2 controller pin connections
#define PS2_DAT 11
#define PS2_CMD 12
#define PS2_SEL 10
#define PS2_CLK 13

PS2X ps2x;
String msg;

void setup() {
  Serial.begin(115200);
  // Configure the PS2 controller to work with Arduino Nano
  int error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, false, false);
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
  else {
    //Handling rest other buttons
    int LY = ps2x.Analog(PSS_LY);
    int LX = ps2x.Analog(PSS_LX);
    int RX = ps2x.Analog(PSS_RX);
    int Speed = ps2x.Button(PSB_SQUARE);
    int Reload = ps2x.Button(PSB_L1);
    int Shoot = ps2x.Button(PSB_R1);
    int Angle_Up = ps2x.Button(PSB_PAD_UP);
    int Angle_Down = ps2x.Button(PSB_PAD_DOWN);
    int Pull = ps2x.Button(PSB_PAD_LEFT);
    int Push = ps2x.Button(PSB_PAD_RIGHT);

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

    // val <velx> <vely> <velw> <pick> <stretch> <tilt> <lockx> <locky>
    msg = String("val") + String(" ") + String(vel_y) + String(" ") + String(vel_x) + String(" ") + String(vel_w) + " 0 " + String(stretch) + String(" ") + String(tilt) + " 0 0";
  }

  // Send this message to ESP07
  Serial.println(msg);
}