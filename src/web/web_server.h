#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <ESP8266WebServer.h>

// Deklaracija globalnog WebServer objekta (definiran u main.cpp)
extern ESP8266WebServer server;

/**
 * Inicijalizira rute web servera i pokreće servis.
 */
void initWebServer();

/**
 * Obrađuje dolazne klijentske zahtjeve. 
 * Treba se pozivati u svakom prolazu loop() funkcije.
 */
void handleClientRequests();

// --- API ENDPOINT HANDLERI ---

void handleRoot();
void handleNotFound();

// Senzori i status
void handleSensorReadings();
void handleUtcTime();

// Kontrola antene (Relej i Stepper)
void handleAntennaStatus();
void handleSetAntenna();
void handleServoAngle();      // Dohvaća trenutni kut stepera
void handleSetAngle();        // Postavlja novi ciljni kut
void handleCalibrateServo();  // Resetira poziciju na 0 stupnjeva

// Upravljanje logovima
void handleGetLogs();
void handleSaveLog();
void handleDeleteLog();
void handleClearAllLogs();

// Geolokacija
void handleSaveUserLocation(); // DODANO: Za spremanje lokacije s preglednika

#endif // WEB_SERVER_H