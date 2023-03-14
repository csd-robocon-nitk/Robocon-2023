const int dirPin = 2;
const int stepPin = 3;
const int stepsPerRevolution = 200;

void setup()
{
	// Declare pins as Outputs
	pinMode(stepPin, OUTPUT);
	pinMode(dirPin, OUTPUT);
}
void loop()
{
  digitalWrite(dirPin, HIGH);
	for(int x = 0; x < stepsPerRevolution*5; x++)
	{
		digitalWrite(stepPin, HIGH);
		delayMicroseconds(700);
		digitalWrite(stepPin, LOW);
		delayMicroseconds(700);
	}
  digitalWrite(dirPin, LOW);
	for(int x = 0; x < stepsPerRevolution*5; x++)
	{
		digitalWrite(stepPin, HIGH);
		delayMicroseconds(700);
		digitalWrite(stepPin, LOW);
		delayMicroseconds(700);
	}
}