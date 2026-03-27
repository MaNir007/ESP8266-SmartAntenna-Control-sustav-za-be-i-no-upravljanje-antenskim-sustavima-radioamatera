/**
 * @file antenna_control.cpp
 * @brief Upravljanje antenskim preklopnikom i rotacijom antene pomoću stepper motora.
 * @details Modul sadrži funkcije za odabir željene antene (putem releja) te
 * postavljanje i kalibriranje kuta usmjerene antene (stepper motor).
 */
#include "antenna_control.h"
#include "../config/config.h" 
#include <AccelStepper.h>

// Inicijalizacija koristeći nazive iz config.h
// Redoslijed za AccelStepper (1, 3, 2, 4) ostaje isti radi ispravnog rada motora
AccelStepper stepper(AccelStepper::FULL4WIRE, STEPPER_IN1, STEPPER_IN3, STEPPER_IN2, STEPPER_IN4);

/**
 * @brief Inicijalizira GPIO pinove za relej i parametre stepper motora (brzina, akceleracija).
 */
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

/**
 * @brief Prebacuje aktivnu antenu promjenom stanja releja.
 * @param type String koji označava vrstu antene (npr. "GP" ili "Yagi").
 */
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

/**
 * @brief Dohvaća trenutno odabranu vrstu antene.
 * @return String koji predstavlja aktivnu antenu.
 */
String getAntennaType() {
    return currentAntenna;
}

/**
 * @brief Postavlja željeni kut antene i prevodi ga u korake za stepper motor.
 * @param angle Ciljani kut u stupnjevima (0 - 360).
 */
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

/**
 * @brief Vraća trenutnu orijentaciju antene u stupnjevima na temelju broja koraka motora.
 * @return Trenutni kut (0 - 360).
 */
int getCurrentServoAngle() {
    // Vraća trenutnu poziciju pretvorenu nazad u stupnjeve
    return map(stepper.currentPosition(), 0, STEPS_PER_REV, 0, 360);
}

/**
 * @brief Rekalibrira trenutnu poziciju motora kao apsolutnu nulu (0 stupnjeva).
 */
void calibrateServo() {
    // Resetira brojač koraka na nulu
    stepper.setCurrentPosition(0);
    desiredAngle = 0;
    Serial.println("Sustav rekalibriran: Trenutni položaj postavljen kao 0 stupnjeva.");
}