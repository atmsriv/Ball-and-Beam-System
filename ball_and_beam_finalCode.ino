// MOTOR & ENCODER PINS
const int ENA = 9;
const int IN1 = 8;
const int IN2 = 7;

const int pinA = 2;
const int pinB = 3;
const float kp = 0.2;

// Pulses per degree
// 540 CPR * 100 Gear Ratio / 360 Degrees = 150
const float pulsesPerDegree = 150.0; 

// SAFETY LIMITS (IN PULSES)
const long DOWN_LIMIT = -35 * pulsesPerDegree; 
const long UP_LIMIT = 35 * pulsesPerDegree;

volatile long position = 0;

void setup()
{
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(pinA), isrA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pinB), isrB, CHANGE);

  Serial.begin(9600);
  
  // HOLD LEVEL MANUALLY, THEN PRESS RESET
  noInterrupts();
  position = 0; 
  interrupts();
}

void loop()
{
  int pwm;
  
  // MOVE DOWNWARD (THE SOFT LANDING)
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  for (pwm = 0; pwm <= 100; pwm += 2)
  {
    if (position <= DOWN_LIMIT)
    break; 
    analogWrite(ENA, pwm);
    delay(10);
  }

  // Quick ramp down to prevent slamming
  for (pwm = 100; pwm >= 0; pwm -= 5)
  {
    analogWrite(ENA, pwm);
    delay(5);
  }
  stopMotor();
  delay(1000);

  // MOVE UPWARD (THE TORQUE PUSH)
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Upwards ramp start at 80
  for (pwm = 80; pwm <= 160; pwm += 5)
  {
    if (position >= UP_LIMIT)
    break; 
    analogWrite(ENA, pwm);
    delay(10);
  }

  // Quick ramp down to protect the rods
  for (pwm = 160; pwm >= 0; pwm -= 5)
  {
    analogWrite(ENA, pwm);
    delay(5);
  }
  stopMotor();
  delay(1000);
}

// ENCODER ISR
void isrA()
{
  if (digitalRead(pinA) == digitalRead(pinB)) position--;
  else position++;
}
void isrB()
{
  if (digitalRead(pinA) != digitalRead(pinB)) position--;
  else position++;
}

void stopMotor()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
}