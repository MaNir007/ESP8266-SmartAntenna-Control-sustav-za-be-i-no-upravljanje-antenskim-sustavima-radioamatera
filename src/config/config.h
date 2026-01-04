// src/config/config.h
#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// --- DEFINICIJE PIN-ova ---
#define RELAY_PIN D2
#define DHT_PIN D1
#define DHT_TYPE DHT22

// --- STEPPER KONFIGURACIJA (28BYJ-48 s ULN2003) ---
// Pinovi prema tvom rasporedu: IN1=D6, IN2=D7, IN3=D5, IN4=D0
#define STEPPER_IN1 D6
#define STEPPER_IN2 D7
#define STEPPER_IN3 D5
#define STEPPER_IN4 D0

// Parametri kretanja (prilagođeno za teret antene)
const int STEPPER_MAX_SPEED = 200;      
const int STEPPER_ACCELERATION = 100;   
const int STEPS_PER_REV = 2048;         

// --- GPS KONFIGURACIJA (Ako se koristi SoftwareSerial) ---
#define GPS_RX_PIN D4 // Primjer, prilagodi ako koristiš drugi pin
#define GPS_TX_PIN D3 // Primjer, prilagodi ako koristiš drugi pin

// --- WiFi PODACI ---
// Maknuli smo 'extern' i stavili vrijednosti ovdje radi preglednosti
const char* const WIFI_SSID = "Speedport-954854";
const char* const WIFI_PASSWORD = "ffnkp35u8sxffe4m";

// --- NTP KONFIGURACIJA ---
const char* const NTP_SERVER = "pool.ntp.org";
const long NTP_OFFSET = 0;
const long NTP_UPDATE_INTERVAL = 60000;

// --- NAZIV DATOTEKE ZA LOGOVE ---
const char* const LOG_FILENAME = "/logs.json";

// --- UKLONJENO ---
// SERVO_PIN i SERVO KALIBRACIJA više nisu potrebni

#endif // CONFIG_H