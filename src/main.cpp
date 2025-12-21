#include <Arduino.h>

// Uključivanje svih header datoteka
#include "config/config.h"
#include "common/common_types.h"
#include "wifi/wifi_manager.h"
#include "sensors/sensor_manager.h"
#include "antenna/antenna_control.h"
#include "time/time_sync.h"
#include "filesystem/fs_utils.h"
#include "logger/log_manager.h"
#include "web/web_server.h"
#include <ESP8266WebServer.h>
#include <AccelStepper.h>

// --- DEFINICIJE GLOBALNIH VARIJABLI ---
// Ovdje definiramo varijable (bez extern) koje će drugi moduli koristiti
String currentAntenna = "GP"; 
int desiredAngle = 0;         
float currentLat = 0.0;
float currentLon = 0.0;
bool gpsValid = false;

// --- EKSTERNE REFERENCE ---
// Ovi objekti su definirani u antenna_control.cpp
extern AccelStepper stepper; 

// --- GLOBALNI OBJEKTI ---

// Hardverski objekti
DHT dht(DHT_PIN, DHT_TYPE); 

// GPS Objekti
SoftwareSerial gpsSerial(GPS_RX_PIN, GPS_TX_PIN); 
TinyGPSPlus gps; 

// Mrežni i vremenski objekti
WiFiUDP ntpUDP; 
NTPClient timeClient(ntpUDP, NTP_SERVER, NTP_OFFSET, NTP_UPDATE_INTERVAL); 

// Web server na portu 80
ESP8266WebServer server(80);

void setup() {
    Serial.begin(115200);
    delay(500); 
    Serial.println("\n--- ANTENSKI SUSTAV SETUP ---");

    // Redoslijed inicijalizacije modula
    initWiFi();             // Spajanje na WiFi
    initLittleFS();         // Sustav datoteka za web stranicu i logove
    initSensors();          // DHT senzor i GPS serijska veza
    initAntennaControl();   // Postavke steper motora i releja
    initTimeSync();         // Dohvat vremena preko NTP-a
    initLogManager();       // Provjera log datoteka
    initWebServer();        // Registracija API ruta i pokretanje servera

    Serial.println("--- SUSTAV SPREMAN ---");
}

void loop() {
    // 1. NAJVIŠI PRIORITET: Stepper motor pokreti
    // Ova funkcija mora se vrtjeti što je brže moguće
    stepper.run(); 

    // 2. Obrada mrežnih zahtjeva
    handleClientRequests(); 

    // 3. Ažuriranje vremena i GPS-a (ne-blokirajuće funkcije)
    updateTime(); 
    updateGpsData(); 

    // yield() dopušta ESP-u da odradi pozadinske WiFi zadatke
    yield(); 
}