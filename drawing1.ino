#include <AccelStepper.h>

// Define stepper motors
AccelStepper motorX1(1, 3, 6); // Step, Dir for X1
AccelStepper motorX2(1, 8, 9); // Step, Dir for X2
AccelStepper motorY(1, 10, 11); // Step, Dir for Y
AccelStepper motorZ(1, 12, 13); // Step, Dir for Z

// Steps per mm for motion
const float stepsPerMM = 50;

// Z-axis positions for pen control
const float penUpPosition = 0.0; // Z=0 mm for pen up
const float penDownPosition = -5.0; // Z=-5 mm for pen down (adjust as needed)

void setup() {
  Serial.begin(9600);

  // Initialize stepper motors
  motorX1.setMaxSpeed(5000);
  motorX1.setAcceleration(1000);
  motorX2.setMaxSpeed(5000);
  motorX2.setAcceleration(1000);
  motorY.setMaxSpeed(3000);
  motorY.setAcceleration(800);
  motorZ.setMaxSpeed(2000);
  motorZ.setAcceleration(500);

  Serial.println("Gantry System Ready with G-code and Z-axis Pen Control!");
}

void loop() {
  if (Serial.available()) {
    String gcode = Serial.readStringUntil('\n');
    executeGCode(gcode);
  }
}

// Function to execute a G-code command
void executeGCode(String gcode) {
  gcode.trim();
  if (gcode.startsWith("G0") || gcode.startsWith("G1")) {
    // Parse coordinates
    float x = parseCoordinate(gcode, 'X');
    float y = parseCoordinate(gcode, 'Y');
    float z = parseCoordinate(gcode, 'Z');
    moveGantry(x, y, z);
  } else if (gcode.startsWith("M3")) {
    moveZ(penDownPosition); // Pen down
    Serial.println("Pen Down");
  } else if (gcode.startsWith("M5")) {
    moveZ(penUpPosition); // Pen up
    Serial.println("Pen Up");
  } else {
    Serial.println("Unknown G-code Command");
  }
}

// Function to parse a coordinate from the G-code
float parseCoordinate(String gcode, char axis) {
  int index = gcode.indexOf(axis);
  if (index != -1) {
    int nextSpace = gcode.indexOf(' ', index);
    return gcode.substring(index + 1, nextSpace).toFloat();
  }
  return NAN;
}

// Function to move gantry synchronously
void moveGantry(float x, float y, float z) {
  if (!isnan(x)) {
    long stepsX = x * stepsPerMM;
    motorX1.moveTo(stepsX);
    motorX2.moveTo(stepsX);
  }
  if (!isnan(y)) {
    long stepsY = y * stepsPerMM;
    motorY.moveTo(stepsY);
  }
  if (!isnan(z)) {
    long stepsZ = z * stepsPerMM;
    motorZ.moveTo(stepsZ);
  }

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

// Function to move Z-axis for pen control
void moveZ(float position) {
  long stepsZ = position * stepsPerMM;
  motorZ.moveTo(stepsZ);

  while (motorZ.distanceToGo() != 0) {
    motorZ.run();
  }
}
