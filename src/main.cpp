// ---------------------------------------------------------------------------
// main.cpp - Glavna ulazna točka (entry point) programa za antenski sustav
// ---------------------------------------------------------------------------
// Ovaj modul inicijalizira sve ostale komponente sustava (WiFi, LittleFS, 
// Web Server, Senzore, Logove itd.) te sadrži glavnu petlju (loop) unutar 
// koje se periodički osvježavaju podaci, pokreće servo motor (stepper) 
// i obrađuju HTTP zahtjevi od strane web klijenta.
// ---------------------------------------------------------------------------

#include <Arduino.h>

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

// ---------------------------------------------------------------------------
// DEFINICIJE GLOBALNIH VARIJABLI
// ---------------------------------------------------------------------------
String currentAntenna = "Yagi"; 
int desiredAngle = 0;          
float currentLat = 0.0;
float currentLon = 0.0;
bool gpsValid = false;

// ---------------------------------------------------------------------------
// NOVO: Varijable za pozadinsko logiranje
// ---------------------------------------------------------------------------
unsigned long zadnjeLogiranje = 0;
const unsigned long intervalLogiranja = 60000; 

// ---------------------------------------------------------------------------
// EKSTERNE REFERENCE
// ---------------------------------------------------------------------------
extern AccelStepper stepper; 

// ---------------------------------------------------------------------------
// GLOBALNI OBJEKTI
// ---------------------------------------------------------------------------
DHT dht(DHT_PIN, DHT_TYPE); 
SoftwareSerial gpsSerial(GPS_RX_PIN, GPS_TX_PIN); 
TinyGPSPlus gps; 
WiFiUDP ntpUDP; 
NTPClient timeClient(ntpUDP, NTP_SERVER, NTP_OFFSET, NTP_UPDATE_INTERVAL); 
ESP8266WebServer server(80);

// ---------------------------------------------------------------------------
// setup() - Početna konfiguracija uređaja
// ---------------------------------------------------------------------------
void setup() {
    Serial.begin(115200);
    delay(500); 
    Serial.println("\n--- ANTENSKI SUSTAV SETUP ---");

    initWiFi();             
    initLittleFS();         
    initSensors();          
    initAntennaControl();   
    initTimeSync();         
    initLogManager();       
    initWebServer();        

    Serial.println("--- SUSTAV SPREMAN ---");
}

// ---------------------------------------------------------------------------
// loop() - Glavna programska petlja
// ---------------------------------------------------------------------------
void loop() {
    stepper.run(); 

    handleClientRequests(); 

    updateTime(); 
    updateGpsData(); 

    if (millis() - zadnjeLogiranje >= intervalLogiranja) {
        zadnjeLogiranje = millis();
        
        float t = getTemperature();
        float h = getHumidity();

        if (t != -999.0 && h != -999.0) {
            String logZapis = String(t) + "," + String(h);
            appendLog("/data_log.csv", logZapis);
            Serial.println("Pozadinski log spremljen: " + logZapis);
        }
    }

    yield(); 
}