// ---------------------------------------------------------------------------
// antenna_control.cpp - Upravljanje antenskim preklopnikom i rotacijom
// ---------------------------------------------------------------------------
// Modul sadrži funkcije za odabir željene antene (putem releja) te
// postavljanje i kalibriranje kuta usmjerene antene (stepper motor).
// ---------------------------------------------------------------------------
#include "antenna_control.h"
#include "../config/config.h" 
#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::FULL4WIRE, STEPPER_IN1, STEPPER_IN3, STEPPER_IN2, STEPPER_IN4);

// ---------------------------------------------------------------------------
// Inicijalizacija GPIO pinova i parametara stepper motora
// ---------------------------------------------------------------------------
void initAntennaControl() {
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW); 
    currentAntenna = "GP";
    Serial.println("Relej inicijaliziran. Antena: " + currentAntenna);

    stepper.setMaxSpeed(STEPPER_MAX_SPEED);     
    stepper.setAcceleration(STEPPER_ACCELERATION); 
    
    desiredAngle = 0;
    stepper.setCurrentPosition(0); 

    Serial.print("Stepper inicijaliziran. Max brzina: ");
    Serial.println(STEPPER_MAX_SPEED);
}

// ---------------------------------------------------------------------------
// Prebacuje aktivnu antenu promjenom stanja releja
// ---------------------------------------------------------------------------
void setAntennaType(const String& type) {
    if (type == "GP") {
        digitalWrite(RELAY_PIN, HIGH);
        currentAntenna = "GP";
    } else if (type == "Yagi") {
        digitalWrite(RELAY_PIN, LOW);
        currentAntenna = "Yagi";
    }
    Serial.println("Relej prebačen na: " + currentAntenna);
}

// ---------------------------------------------------------------------------
// Dohvaća trenutno odabranu vrstu antene
// ---------------------------------------------------------------------------
String getAntennaType() {
    return currentAntenna;
}

// ---------------------------------------------------------------------------
// Postavlja željeni kut antene i prevodi ga u korake za stepper motor
// ---------------------------------------------------------------------------
void setServoAngle(int angle) {
    if (angle < 0 || angle > 360) {
        Serial.println("Kut izvan raspona: " + String(angle));
        return;
    }

    desiredAngle = angle;

    long targetStep = map(angle, 0, 360, 0, STEPS_PER_REV);

    stepper.moveTo(targetStep);
    
    Serial.printf("Cilj: %d stupnjeva, Ciljni korak: %ld\n", desiredAngle, targetStep);
}

// ---------------------------------------------------------------------------
// Vraća trenutnu orijentaciju antene u stupnjevima
// ---------------------------------------------------------------------------
int getCurrentServoAngle() {
    return map(stepper.currentPosition(), 0, STEPS_PER_REV, 0, 360);
}

// ---------------------------------------------------------------------------
// Rekalibrira trenutnu poziciju motora kao apsolutnu nulu
// ---------------------------------------------------------------------------
void calibrateServo() {
    stepper.setCurrentPosition(0);
    desiredAngle = 0;
    Serial.println("Sustav rekalibriran: Trenutni položaj postavljen kao 0 stupnjeva.");
}