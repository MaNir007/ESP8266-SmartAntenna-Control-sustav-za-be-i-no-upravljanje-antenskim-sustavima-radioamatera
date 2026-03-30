// ---------------------------------------------------------------------------
// antenna_control.h - Sučelje za upravljanje antenom i stepper motorom
// ---------------------------------------------------------------------------
// Definira funkcije za inicijalizaciju, promjenu tipa antene, 
// postavljanje kuta i rekalibraciju stepper motora.
// ---------------------------------------------------------------------------
#ifndef ANTENNA_CONTROL_H
#define ANTENNA_CONTROL_H

#include <Arduino.h>
#include <AccelStepper.h> 


extern AccelStepper stepper; 
extern String currentAntenna;
extern int desiredAngle;

void initAntennaControl();
void setAntennaType(const String& type);
String getAntennaType();

void setServoAngle(int angle); 
int getCurrentServoAngle();
void calibrateServo();

#endif // ANTENNA_CONTROL_H