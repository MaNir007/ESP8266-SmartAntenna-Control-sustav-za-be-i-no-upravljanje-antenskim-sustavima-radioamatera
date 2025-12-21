#include "antenna_control.h"
#include "../config/config.h" 
#include <AccelStepper.h>

// Inicijalizacija koristeći nazive iz config.h
// Redoslijed za AccelStepper (1, 3, 2, 4) ostaje isti radi ispravnog rada motora
AccelStepper stepper(AccelStepper::FULL4WIRE, STEPPER_IN1, STEPPER_IN3, STEPPER_IN2, STEPPER_IN4);

void initAntennaControl() {
    // Korištenje definicije RELAY_PIN iz config.h
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW); 
    currentAntenna = "GP";
    Serial.println("Relej inicijaliziran. Antena: " + currentAntenna);

    // Postavke brzine preuzete iz config.h
    stepper.setMaxSpeed(STEPPER_MAX_SPEED);     
    stepper.setAcceleration(STEPPER_ACCELERATION); 
    
    desiredAngle = 0;
    // Postavlja trenutnu fiziku poziciju kao nulu (bitno kod paljenja)
    stepper.setCurrentPosition(0); 

    Serial.print("Stepper inicijaliziran. Max brzina: ");
    Serial.println(STEPPER_MAX_SPEED);
}

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

String getAntennaType() {
    return currentAntenna;
}

void setServoAngle(int angle) {
    if (angle < 0 || angle > 360) {
        Serial.println("Kut izvan raspona: " + String(angle));
        return;
    }

    desiredAngle = angle;

    // Korištenje STEPS_PER_REV iz config.h za precizan izračun
    long targetStep = map(angle, 0, 360, 0, STEPS_PER_REV);

    stepper.moveTo(targetStep);
    
    Serial.printf("Cilj: %d stupnjeva, Ciljni korak: %ld\n", desiredAngle, targetStep);
}

int getCurrentServoAngle() {
    // Vraća trenutnu poziciju pretvorenu nazad u stupnjeve
    return map(stepper.currentPosition(), 0, STEPS_PER_REV, 0, 360);
}

void calibrateServo() {
    // Resetira brojač koraka na nulu
    stepper.setCurrentPosition(0);
    desiredAngle = 0;
    Serial.println("Sustav rekalibriran: Trenutni položaj postavljen kao 0 stupnjeva.");
}