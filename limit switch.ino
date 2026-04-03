#include <AccelStepper.h>

// Define stepper motors
AccelStepper motorX1(1, 3, 6); // Step, Dir for X1
AccelStepper motorX2(1, 8, 9); // Step, Dir for X2 (synchronous with X1)
AccelStepper motorY(1, 10, 11); // Step, Dir for Y
AccelStepper motorZ(1, 12, 13); // Step, Dir for Z

// Limit switch pins (NC configuration)
#define LIMIT_X_MIN 4
#define LIMIT_X_MAX 5
#define LIMIT_Y 7
#define LIMIT_Z 2

// Steps per mm for motion (adjust as needed)
const float stepsPerMM = 50;

void setup() {
  Serial.begin(9600);

  // Set maximum speed and acceleration
  motorX1.setMaxSpeed(5000);
  motorX1.setAcceleration(1000);
  motorX2.setMaxSpeed(5000);
  motorX2.setAcceleration(1000);
  motorY.setMaxSpeed(3000);
  motorY.setAcceleration(800);
  motorZ.setMaxSpeed(2000);
  motorZ.setAcceleration(500);

  // Configure limit switches as inputs with pull-ups
  pinMode(LIMIT_X_MIN, INPUT_PULLUP);
  pinMode(LIMIT_X_MAX, INPUT_PULLUP);
  pinMode(LIMIT_Y, INPUT_PULLUP);
  pinMode(LIMIT_Z, INPUT_PULLUP);

  Serial.println("Starting Homing...");

  // Home all axes
  homeAxis(motorX1, motorX2, LIMIT_X_MIN);
  homeAxis(motorY, nullptr, LIMIT_Y);
  homeAxis(motorZ, nullptr, LIMIT_Z);

  Serial.println("Homing Complete!");

  // Test motion
  moveGantry(50, 50, -30);
  delay(2000);
  moveGantry(0, 0, 0);
}

void loop() {
  // Intentionally left empty
}

// Function to home an axis
void homeAxis(AccelStepper &motor1, AccelStepper *motor2, int limitSwitchPin) {
  motor1.setSpeed(-500); // Move towards the limit switch
  if (motor2) motor2->setSpeed(-500);

  while (digitalRead(limitSwitchPin) == LOW) {
    motor1.runSpeed();
    if (motor2) motor2->runSpeed();
  }

  // Back off slightly
  motor1.setCurrentPosition(0);
  motor1.setSpeed(500);
  motor1.move(10);
  while (motor1.distanceToGo() != 0) {
    motor1.run();
  }

  if (motor2) {
    motor2->setCurrentPosition(0);
    motor2->setSpeed(500);
    motor2->move(10);
    while (motor2->distanceToGo() != 0) {
      motor2->run();
    }
  }

  motor1.setCurrentPosition(0); // Set the home position
  if (motor2) motor2->setCurrentPosition(0);
}

// Function to move gantry synchronously
void moveGantry(float x, float y, float z) {
  long stepsX = x * stepsPerMM;
  long stepsY = y * stepsPerMM;
  long stepsZ = z * stepsPerMM;

  motorX1.moveTo(stepsX);
  motorX2.moveTo(stepsX);
  motorY.moveTo(stepsY);
  motorZ.moveTo(stepsZ);

  while (motorX1.distanceToGo() != 0 || motorX2.distanceToGo() != 0 ||
         motorY.distanceToGo() != 0 || motorZ.distanceToGo() != 0) {
    motorX1.run();
    motorX2.run();
    motorY.run();
    motorZ.run();
  }

  Serial.print("Moved to X: ");
  Serial.print(x);
  Serial.print(" Y: ");
  Serial.print(y);
  Serial.print(" Z: ");
  Serial.println(z);
}
