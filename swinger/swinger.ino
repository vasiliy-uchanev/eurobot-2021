#include <Arduino.h>
#define touchedRoofInterruptPin 2
#define touchedFloorInterruptPin 3
#define dirPin 4
#define stepPin 5
#define testHighPin 6
#define grabPin 7
#define dropPin 8

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
bool debugEnabled = true;

void setup()
{
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  
  pinMode(grabPin, INPUT);
  pinMode(dropPin, INPUT);

  pinMode(testHighPin, OUTPUT);
  digitalWrite(testHighPin, HIGH);

  pinMode(touchedRoofInterruptPin, INPUT_PULLUP);
  pinMode(touchedFloorInterruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(touchedRoofInterruptPin), touchedRoofInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(touchedFloorInterruptPin), touchedFloorInterrupt, FALLING);

  if (debugEnabled)
  {
    Serial.begin(115200);
    Serial.println("initialized");
  }
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
  if (mode == idle) {
    checkMothershipConnections();
  }
  if (mode == grab)
  {
    performDownUpCycle();
  }
  if (mode == drop)
  {
    performUpDownCycle();
  }
}

void calibrate()
{
  debugPrint("Calibrating...");
  touchedFloor = false;
  setDirectionDown();
  debugPrint("Going down...");
  while (!touchedFloor)
  {
    spin(calibrationStep);
  }
  debugPrint("Done.");
  delay(100);
  currentPosition = 0;
  touchedRoof = false;
  setDirectionUp();
  debugPrint("Going up...");
  while (!touchedRoof)
  {
    spin(calibrationStep);
  }
  debugPrint("Done.");
  delay(100);
  topPosition = currentPosition;
  idlePosition = currentPosition * 0.75;
  debugPrint("top position: ");debugPrint(String(topPosition));
  debugPrint("idle position: ");debugPrint(String(idlePosition));
  mode = resetPosition;
}

void checkMothershipConnections() {
  if(digitalRead(grabPin) == HIGH) {
    mode = grab;
  }
  else if (digitalRead(dropPin) == HIGH) {
    mode = drop;
  }
}

void touchedFloorInterrupt()
{
  touchedFloor = true;
}

void touchedRoofInterrupt()
{
  touchedRoof = true;
}

void performDownUpCycle()
{
  setDirectionUp();
  spin(stepsUpCount);
  delay(100);
  setDirectionDown();
  spin(stepsUpCount);
  mode = idle;
}

void performUpDownCycle()
{
  setDirectionDown();
  spin(stepsDownCount);
  delay(100);
  setDirectionUp();
  spin(stepsDownCount);
  mode = idle;
}

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
  digitalWrite(dirPin, HIGH);
  movingUp = false;
}

void setDirectionUp()
{
  digitalWrite(dirPin, LOW);
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

void debugPrint(String message)
{
  if (debugEnabled)
  {
    Serial.println(message);
  }
}
