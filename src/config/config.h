// ---------------------------------------------------------------------------
// config.h - Globalna konfiguracija sustava (pinovi i konstante)
// ---------------------------------------------------------------------------
// Sadrži mapiranje GPIO pinova, konfiguraciju za WiFi, stepper motor, 
// NTP server, senzore i putanje datotečnog sustava.
// ---------------------------------------------------------------------------
#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ---------------------------------------------------------------------------
// DEFINICIJE PIN-ova
// ---------------------------------------------------------------------------
#define RELAY_PIN D2
#define DHT_PIN D1
#define DHT_TYPE DHT22

// ---------------------------------------------------------------------------
// STEPPER KONFIGURACIJA (28BYJ-48 s ULN2003)
// ---------------------------------------------------------------------------

#define STEPPER_IN1 D6
#define STEPPER_IN2 D7
#define STEPPER_IN3 D5
#define STEPPER_IN4 D0


const int STEPPER_MAX_SPEED = 200;      
const int STEPPER_ACCELERATION = 100;   
const int STEPS_PER_REV = 2048;         

// ---------------------------------------------------------------------------
// GPS KONFIGURACIJA
// ---------------------------------------------------------------------------
#define GPS_RX_PIN D4 
#define GPS_TX_PIN D3 

// ---------------------------------------------------------------------------
// WiFi PODACI
// ---------------------------------------------------------------------------

const char* const WIFI_SSID = "Speedport-954854";
const char* const WIFI_PASSWORD = "ffnkp35u8sxffe4m";

// ---------------------------------------------------------------------------
// NTP KONFIGURACIJA
// ---------------------------------------------------------------------------
const char* const NTP_SERVER = "pool.ntp.org";
const long NTP_OFFSET = 0;
const long NTP_UPDATE_INTERVAL = 60000;

// ---------------------------------------------------------------------------
// NAZIV DATOTEKE ZA LOGOVE
// ---------------------------------------------------------------------------
const char* const LOG_FILENAME = "/logs.json";



#endif // CONFIG_H