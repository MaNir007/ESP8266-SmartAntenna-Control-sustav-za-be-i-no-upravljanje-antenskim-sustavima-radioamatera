#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include <DHT.h> 
#include <SoftwareSerial.h> 
#include <TinyGPSPlus.h>    

// Deklaracija globalnih objekata (definirani u main.cpp)
extern DHT dht; 
extern SoftwareSerial gpsSerial;
extern TinyGPSPlus gps;

// Deklaracija globalnih varijabli za GPS podatke
extern float currentLat;
extern float currentLon;
extern bool gpsValid;

// --- GPS PIN KONFIGURACIJA ---
// VAŽNO: Pomaknuto s D5/D6 na D4/D3 jer stepper koristi D5/D6/D7/D0
#define GPS_RX_PIN D4 // GPIO2 - Spoji TX pin GPS-a ovdje
#define GPS_TX_PIN D3 // GPIO0 - Spoji RX pin GPS-a ovdje
#define GPS_BAUD_RATE 9600 

// Deklaracije funkcija za senzore
void initSensors();
float getTemperature();
float getHumidity();
float getVoltage(); 

// Deklaracije funkcija za GPS
void initGps(); 
void updateGpsData(); 
bool isGpsValid();
float getGpsLatitude();
float getGpsLongitude();

#endif // SENSOR_MANAGER_H