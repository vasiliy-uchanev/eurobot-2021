#include <Arduino.h>
#define touchedRoofInterruptPin 2
#define touchedFloorInterruptPin 3
#define dirPin 4
#define stepPin 5
#define testHighPin 6

#define stepsDownCount 4000
#define stepsUpCount 1000
#define calibrationStep 10
#define delayMicroSecs 500

enum swingerMode
{
  calibration,
  resetPosition,
  idle,
  grab,
  drop
};

swingerMode mode = calibration;
bool touchedFloor = false;
bool touchedRoof = false;
int topPosition = 0;
int idlePosition = 0;
int currentPosition = 0;
bool movingUp = false;

void setup()
{
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  pinMode(testHighPin, OUTPUT);
  digitalWrite(testHighPin, HIGH);

  pinMode(touchedRoofInterruptPin, INPUT_PULLUP);
  pinMode(touchedFloorInterruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(touchedRoofInterruptPin), touchedRoofInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(touchedFloorInterruptPin), touchedFloorInterrupt, FALLING);
}

void loop()
{
  if (mode == calibration)
  {
    calibrate();
  }
  if (mode == resetPosition)
  {
    moveToTheIdlePoint();
  }
  // if (mode == grab)
  // {
  //   performDownUpCycle();
  // }
  // if (mode == drop)
  // {
  //   performUpDownCycle();
  // }
}

void calibrate()
{
  touchedFloor = false;
  setDirectionDown();
  while (!touchedFloor)
  {
    spin(calibrationStep);
  }
  currentPosition = 0;
  touchedRoof = false;
  setDirectionUp();
  while (!touchedRoof)
  {
    spin(calibrationStep);
  }
  topPosition = currentPosition;
  idlePosition = currentPosition * 0.75;
  mode = resetPosition;
}

void touchedFloorInterrupt()
{
  touchedFloor = true;
}

void touchedRoofInterrupt()
{
  touchedRoof = true;
}

// void handleGrabCall()
// {
//   mode = grab;
// }

// void handleDropCall()
// {
//   mode = drop;
// }

// void performDownUpCycle()
// {
//   setDirectionUp();
//   spin(stepsUpCount);
//   delay(100);
//   setDirectionDown();
//   spin(stepsUpCount);
//   mode = idle;
// }

// void performUpDownCycle()
// {
//   setDirectionDown();
//   spin(stepsDownCount);
//   delay(100);
//   setDirectionUp();
//   spin(stepsDownCount);
//   mode = idle;
// }

void spin(int steps)
{
  for (int i = 0; i < steps; i++)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(delayMicroSecs);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(delayMicroSecs);
  }
  currentPosition = movingUp ? currentPosition + steps : currentPosition - steps;
}

void setDirectionDown()
{
  digitalWrite(dirPin, LOW);
  movingUp = false;
}

void setDirectionUp()
{
  digitalWrite(dirPin, HIGH);
  movingUp = true;
}

void moveToTheIdlePoint()
{
  if (currentPosition > idlePosition)
  {
    setDirectionDown();
    spin(currentPosition - idlePosition);
  }
  else
  {
    setDirectionUp();
    spin(idlePosition - currentPosition);
  }
  mode = idle;
}
