// ---------------------------------------------------------------------------
// sensor_manager.h - Sučelje za upravljanje senzorima (DHT, GPS, ADC)
// ---------------------------------------------------------------------------
// Definira globalne objekte, varijable i funkcije za prikupljanje podataka
// s okolišnih senzora i GPS modula.
// ---------------------------------------------------------------------------
#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include <DHT.h> 
#include <SoftwareSerial.h> 
#include <TinyGPSPlus.h>    

extern DHT dht; 
extern SoftwareSerial gpsSerial;
extern TinyGPSPlus gps;

// Deklaracija globalnih varijabli za GPS podatke
extern float currentLat;
extern float currentLon;
extern bool gpsValid;

// ---------------------------------------------------------------------------
// GPS PIN KONFIGURACIJA
// ---------------------------------------------------------------------------
#define GPS_RX_PIN D4 
#define GPS_TX_PIN D3 
#define GPS_BAUD_RATE 9600 

// Deklaracije funkcija za senzore
void initSensors();
float getTemperature();
float getHumidity();
float getVoltage(); 

void initGps(); 
void updateGpsData(); 
bool isGpsValid();
float getGpsLatitude();
float getGpsLongitude();

#endif // SENSOR_MANAGER_H