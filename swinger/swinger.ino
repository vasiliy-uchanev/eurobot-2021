#define interrruptDropPin 2
#define interruptGrabPin 3
#define dirPin 4
#define stepPin 5
#define testHighPin 6

#define stepsDownCount 4000
#define stepsUpCount 1000
#define delayMicroSecs 500

enum swingerMode {
  idle,
  grab,
  drop
};

swingerMode mode = idle;

void setup() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(testHighPin, OUTPUT);
  digitalWrite(testHighPin, HIGH);
  attachInterrupt(digitalPinToInterrupt(interrruptDropPin), handleDropCall, RISING);
  attachInterrupt(digitalPinToInterrupt(interruptGrabPin), handleGrabCall, RISING);
}

void loop() {
  if(mode == grab) {
    performDownUpCycle();
    mode = idle;
  }
  if(mode == drop) {
    performUpDownCycle();
    mode = idle;
  }
}

void handleGrabCall() {
  mode = grab;
}

void handleDropCall() {
  mode = drop;
}


void performDownUpCycle() {
  digitalWrite(dirPin, HIGH);
  spin(stepsUpCount);
  delay(100);
  digitalWrite(dirPin, LOW);
  spin(stepsUpCount);
}

void performUpDownCycle() {
  digitalWrite(dirPin, LOW);
  spin(stepsDownCount);
  delay(100);
  digitalWrite(dirPin, HIGH);
  spin(stepsDownCount);
}

void spin(int steps) {
  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(delayMicroSecs);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(delayMicroSecs);
  }
}
