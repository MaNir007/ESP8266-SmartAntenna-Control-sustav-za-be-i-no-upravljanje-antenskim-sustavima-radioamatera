// ---------------------------------------------------------------------------
// web_server.h - Sučelje za konfiguraciju web poslužitelja i API-ja
// ---------------------------------------------------------------------------
// Definira funkcije za inicijalizaciju servera, obradu klijentskih zahtjeva
// i handlere za sve REST API endpointe sustava.
// ---------------------------------------------------------------------------
#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <ESP8266WebServer.h>

extern ESP8266WebServer server;

// ---------------------------------------------------------------------------
// Inicijalizira rute web servera i pokreće servis
// ---------------------------------------------------------------------------
void initWebServer();

// ---------------------------------------------------------------------------
// Obrada dolaznih mrežnih zahtjeva (HTTP klijenti)
// ---------------------------------------------------------------------------
void handleClientRequests();

// ---------------------------------------------------------------------------
// API ENDPOINT HANDLERI
// ---------------------------------------------------------------------------

void handleRoot();
void handleNotFound();


void handleSensorReadings();
void handleUtcTime();


void handleAntennaStatus();
void handleSetAntenna();
void handleServoAngle();      
void handleSetAngle();        
void handleCalibrateServo();  


void handleGetLogs();
void handleSaveLog();
void handleDeleteLog();
void handleClearAllLogs();

void handleSaveUserLocation(); 

#endif // WEB_SERVER_H