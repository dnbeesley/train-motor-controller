#include <Arduino.h>
#include <Wire.h>

#define A_DIRECTION 12
#define A_PWM 3
#define A_BRAKE 9
#define A_CURRENT_SENSING A0

#define B_DIRECTION 13
#define B_PWM 11
#define B_BRAKE 8
#define B_CURRENT_SENSING A1

void setup()
{
  Wire.begin(0x41);
  Wire.onReceive(setState);
  Wire.onRequest(getCurrent);
#ifdef _DEBUG
  Serial.begin(9600);
  Serial.println("Listening on 0x41");
#endif
}

void getCurrent()
{
  uint8_t state[2];
  state[0] = analogRead(A_CURRENT_SENSING) / 4;
  state[1] = analogRead(A_CURRENT_SENSING) / 4;
  Wire.write(state, 2);
}

void loop()
{
}

void setState(int available)
{
  uint8_t state[3] = {0, 0, 0};
  uint8_t i;

#ifdef _DEBUG
  Serial.print("Received: ");
  Serial.print(available, HEX);
  Serial.println(" bytes");
#endif

  for (i = 0; i < 3; i++)
  {
    if (i < available)
    {
      state[i] = (uint8_t)Wire.read();
    }
    else
    {
      state[i] = 0;
    }
  }

  while (i < available)
  {
    Wire.read();
  }

#ifdef _DEBUG
  for (i = 0; i < 3; i++)
  {
    Serial.print(i);
    Serial.print(" = ");
    Serial.println(state[i]);
  }
#endif

  if ((state[0] & 0x3) == 0x3)
  {
    digitalWrite(A_BRAKE, HIGH);
#ifdef _DEBUG
    Serial.println("Motor A braking");
#endif
  }
  else if (state[0] & 0x1)
  {
    digitalWrite(A_BRAKE, LOW);
    digitalWrite(A_DIRECTION, LOW);
    analogWrite(A_PWM, state[1]);
#ifdef _DEBUG
    Serial.print("Motor A moving forward with speed: ");
    Serial.println(state[1]);
#endif
  }
  else if (state[0] & 0x2)
  {
    digitalWrite(A_BRAKE, LOW);
    digitalWrite(A_DIRECTION, HIGH);
    analogWrite(A_PWM, state[1]);
#ifdef _DEBUG
    Serial.print("Motor A moving backward with speed: ");
    Serial.println(state[1]);
#endif
  }
  else
  {
    digitalWrite(A_BRAKE, LOW);
    digitalWrite(A_DIRECTION, LOW);
    analogWrite(A_PWM, 0);
#ifdef _DEBUG
    Serial.println("Motor A stopped");
#endif
  }

  if ((state[0] & 0xC) == 0xC)
  {
    digitalWrite(B_BRAKE, HIGH);
#ifdef _DEBUG
    Serial.println("Motor B braking");
#endif
  }
  else if (state[0] & 0x4)
  {
    digitalWrite(B_BRAKE, LOW);
    digitalWrite(B_DIRECTION, LOW);
    analogWrite(B_PWM, state[2]);
#ifdef _DEBUG
    Serial.print("Motor B moving forward with speed: ");
    Serial.println(state[2]);
#endif
  }
  else if (state[0] & 0x8)
  {
    digitalWrite(B_BRAKE, LOW);
    digitalWrite(B_DIRECTION, HIGH);
    analogWrite(B_PWM, state[2]);
#ifdef _DEBUG
    Serial.print("Motor B moving backward with speed: ");
    Serial.println(state[2]);
#endif
  }
  else
  {
    digitalWrite(B_BRAKE, LOW);
    digitalWrite(B_DIRECTION, LOW);
    analogWrite(B_PWM, 0);
#ifdef _DEBUG
    Serial.println("Motor B stopped");
#endif
  }
}
