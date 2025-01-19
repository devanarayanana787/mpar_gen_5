#include <AccelStepper.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Define stepper motors
AccelStepper motorX1(1, 3, 6); // Step, Dir for X1
AccelStepper motorX2(1, 8, 9); // Step, Dir for X2 (synchronous with X1)
AccelStepper motorY(1, 10, 11); // Step, Dir for Y
AccelStepper motorZ(1, 12, 13); // Step, Dir for Z

// Steps per mm for motion (adjust as needed)
const float stepsPerMM = 50;

// Initialize the LCD with I2C address 0x27 and 16x2 dimensions
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin for the limit switch
const int limitSwitchPin = 2;

// State flags
volatile bool homingTriggered = false;
bool homingComplete = false; // Indicates if homing is completed

void setup() {
  Serial.begin(9600); // Initialize serial communication
  pinMode(limitSwitchPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(limitSwitchPin), limitSwitchISR, FALLING);

  lcd.init();
  lcd.backlight();
  lcd.print("Delta Automations");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();

  motorX1.setMaxSpeed(5000);
  motorX1.setAcceleration(1000);
  motorX2.setMaxSpeed(5000);
  motorX2.setAcceleration(1000);
  motorY.setMaxSpeed(3000);
  motorY.setAcceleration(800);
  motorZ.setMaxSpeed(2000);
  motorZ.setAcceleration(500);

  lcd.print("System Ready!");
  delay(2000);
  lcd.clear();

  homeGantry(); // Home the gantry to origin
}

void loop() {
  if (!homingComplete) {
    lcd.print("Homing Required");
    Serial.println("Homing Required");
    return;
  }

  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n'); // Read incoming data
    parseAndMove(input); // Parse and execute motion
  }
}

// Interrupt Service Routine for the limit switch
void limitSwitchISR() {
  if (!homingComplete) {
    homingTriggered = true;
  }
}

// Function to home the gantry
void homeGantry() {
  lcd.print("Homing...");
  Serial.println("Homing...");
  motorX1.setSpeed(-1000);
  motorX2.setSpeed(-1000);

  while (!homingTriggered) {
    motorX1.runSpeed();
    motorX2.runSpeed();
  }

  motorX1.stop();
  motorX2.stop();
  homingTriggered = false;

  motorX1.setCurrentPosition(0);
  motorX2.setCurrentPosition(0);
  motorY.setCurrentPosition(0);
  motorZ.setCurrentPosition(0);

  lcd.clear();
  lcd.print("Homed to Origin");
  Serial.println("Gantry Homed to Origin");
  delay(2000);

  detachInterrupt(digitalPinToInterrupt(limitSwitchPin));
  homingComplete = true;
}

// Parse received data and move the gantry
void parseAndMove(String input) {
  float x = 0, y = 0, z = 0;

  int xIndex = input.indexOf('X');
  int yIndex = input.indexOf('Y');
  int zIndex = input.indexOf('Z');

  if (xIndex != -1) x = input.substring(xIndex + 1, input.indexOf(' ', xIndex)).toFloat();
  if (yIndex != -1) y = input.substring(yIndex + 1, input.indexOf(' ', yIndex)).toFloat();
  if (zIndex != -1) z = input.substring(zIndex + 1).toFloat();

  moveGantry(x, y, z);
}

void moveGantry(float x, float y, float z) {
  long stepsX = x * stepsPerMM;
  long stepsY = y * stepsPerMM;
  long stepsZ = z * stepsPerMM;

  motorX1.moveTo(stepsX);
  motorX2.moveTo(stepsX);
  motorY.moveTo(stepsY);
  motorZ.moveTo(stepsZ);

  lcd.clear();
  lcd.print("Moving to:");
  lcd.setCursor(0, 1);
  lcd.print("X:");
  lcd.print(x);
  lcd.print(" Y:");
  lcd.print(y);
  lcd.print(" Z:");
  lcd.print(z);

  while (motorX1.distanceToGo() != 0 || motorX2.distanceToGo() != 0 || 
         motorY.distanceToGo() != 0 || motorZ.distanceToGo() != 0) {
    motorX1.run();
    motorX2.run();
    motorY.run();
    motorZ.run();
  }

  lcd.clear();
  lcd.print("Reached Pos:");
  lcd.setCursor(0, 1);
  lcd.print("X:");
  lcd.print(x);
  lcd.print(" Y:");
  lcd.print(y);
  lcd.print(" Z:");
  lcd.print(z);

  Serial.print("Moved to X: ");
  Serial.print(x);
  Serial.print(" Y: ");
  Serial.print(y);
  Serial.print(" Z: ");
  Serial.println(z);
}
