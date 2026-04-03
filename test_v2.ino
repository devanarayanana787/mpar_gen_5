//initial commit ---Krishnapriya R 
//TEST DONE 
//THE LIMIT SWITCH ONLY ACTIVATES THE SINGLE TIME HITTING THE ORIGIN AND CALIBIRATING ITSELF TO THE ORIGIN 0,0,0



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
  Serial.begin(9600);

  // Initialize limit switch pin as input with pull-up resistor
  pinMode(limitSwitchPin, INPUT_PULLUP);

  // Attach interrupt to the limit switch pin for homing
  attachInterrupt(digitalPinToInterrupt(limitSwitchPin), limitSwitchISR, FALLING);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.print("Delta Automations");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();

  // Set maximum speed and acceleration for all motors
  motorX1.setMaxSpeed(5000);  // Maximum speed (steps per second)
  motorX1.setAcceleration(1000); // Smooth acceleration
  motorX2.setMaxSpeed(5000);
  motorX2.setAcceleration(1000);
  motorY.setMaxSpeed(3000);
  motorY.setAcceleration(800);
  motorZ.setMaxSpeed(2000);
  motorZ.setAcceleration(500);

  lcd.print("System Ready!");
  delay(2000);
  lcd.clear();

  // Home to the limit switch
  homeGantry();
}

void loop() {
  // If homing is not complete, do not execute further commands
  if (!homingComplete) {
    lcd.print("Homing Required");
    Serial.println("Homing Required");
    return;
  }

  // Example motion after homing
  moveGantry(50, 50, -30); // Move to a new position
  delay(2000);

  moveGantry(0, 0, 0); // Return to the origin
  delay(2000);
}

// Interrupt Service Routine for the limit switch
void limitSwitchISR() {
  if (!homingComplete) {
    homingTriggered = true; // Only respond during the homing phase
  }
}

// Function to home the gantry
void homeGantry() {
  lcd.print("Homing...");
  Serial.println("Homing...");

  // Move X-axis motors towards the limit switch (negative steps)
  motorX1.setSpeed(-1000); // Move left slowly
  motorX2.setSpeed(-1000); // Synchronous with X1

  while (!homingTriggered) {
    // Run the motors during homing
    motorX1.runSpeed();
    motorX2.runSpeed();
  }

  // Stop the motors once the limit switch is triggered
  motorX1.stop();
  motorX2.stop();

  // Reset the homing flag
  homingTriggered = false;

  // Reset positions to origin
  motorX1.setCurrentPosition(0);
  motorX2.setCurrentPosition(0);
  motorY.setCurrentPosition(0);
  motorZ.setCurrentPosition(0);

  lcd.clear();
  lcd.print("Homed to Origin");
  Serial.println("Gantry Homed to Origin");
  delay(2000);

  // Detach interrupt after homing is complete
  detachInterrupt(digitalPinToInterrupt(limitSwitchPin));
  homingComplete = true; // Mark homing as complete
}

// Function to move gantry synchronously
void moveGantry(float x, float y, float z) {
  // Convert mm to steps
  long stepsX = x * stepsPerMM;
  long stepsY = y * stepsPerMM;
  long stepsZ = z * stepsPerMM;

  // Set target positions for all motors
  motorX1.moveTo(stepsX);
  motorX2.moveTo(stepsX); // X2 moves synchronously with X1
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

  // Run all motors until they reach their targets
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
