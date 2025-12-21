#ifndef ANTENNA_CONTROL_H
#define ANTENNA_CONTROL_H

#include <Arduino.h>
#include <AccelStepper.h> // Zamjena za Servo.h

// Deklaracija globalnih objekata i varijabli
extern AccelStepper stepper; // Bit će definiran u main.cpp ili antenna_control.cpp
extern String currentAntenna;
extern int desiredAngle;

void initAntennaControl();
void setAntennaType(const String& type);
String getAntennaType();

// Zadržavamo ime funkcije radi kompatibilnosti s ostatkom koda, 
// ali sada upravlja stepperom
void setServoAngle(int angle); 
int getCurrentServoAngle();
void calibrateServo();

#endif // ANTENNA_CONTROL_H