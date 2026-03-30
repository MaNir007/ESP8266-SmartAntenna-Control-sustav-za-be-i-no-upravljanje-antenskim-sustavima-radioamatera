// ---------------------------------------------------------------------------
// sensor_manager.cpp - Upravljanje senzorima (DHT22, GPS, napon)
// ---------------------------------------------------------------------------
// Modul pruža apstrakciju za dohvat podataka s različitih senzora.
// Pokreće GPS komunikaciju i čitanje okolišnih uvjeta poput temperature.
// ---------------------------------------------------------------------------
#include "sensor_manager.h"
#include "../config/config.h" 
#include <DHT.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// ---------------------------------------------------------------------------
// EXTERN DEKLARACIJE
// ---------------------------------------------------------------------------

extern DHT dht;
extern SoftwareSerial gpsSerial;
extern TinyGPSPlus gps;
extern float currentLat;
extern float currentLon;
extern bool gpsValid;

static unsigned long lastGpsPrint = 0; 

// ---------------------------------------------------------------------------
// Inicijalizira sve senzore sustava
// ---------------------------------------------------------------------------
void initSensors() {
    dht.begin();
    Serial.println("DHT22 senzor inicijaliziran.");
    
    initGps(); 
}

// ---------------------------------------------------------------------------
// Očitava temperaturu s DHT senzora
// ---------------------------------------------------------------------------
float getTemperature() {
    float temperature = dht.readTemperature();
    if (isnan(temperature)) {
        Serial.println("Greška: DHT očitavanje (Temp) neuspješno!");
        return -999.0;
    }
    return temperature;
}

// ---------------------------------------------------------------------------
// Očitava vlažnost s DHT senzora
// ---------------------------------------------------------------------------
float getHumidity() {
    float humidity = dht.readHumidity();
    if (isnan(humidity)) {
        Serial.println("Greška: DHT očitavanje (Hum) neuspješno!");
        return -999.0;
    }
    return humidity;
}

// ---------------------------------------------------------------------------
// Izračunava napon sustava (ADC A0)
// ---------------------------------------------------------------------------
float getVoltage() {
    int raw = analogRead(A0);
    return (raw / 1023.0) * 3.3; 
}

// ---------------------------------------------------------------------------
// GPS FUNKCIJE
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Inicijalizira serijsku komunikaciju za GPS modul
// ---------------------------------------------------------------------------
void initGps() {
    #ifndef GPS_BAUD_RATE
        #define GPS_BAUD_RATE 9600
    #endif

    gpsSerial.begin(GPS_BAUD_RATE);
    Serial.println("GPS SoftwareSerial pokrenut na 9600 baud.");
}

// ---------------------------------------------------------------------------
// Čita dostupne podatke iz GPS-a i ažurira koordinate
// ---------------------------------------------------------------------------
void updateGpsData() {
    while (gpsSerial.available()) {
        gps.encode(gpsSerial.read());
    }


    if (gps.location.isUpdated()) {
        if (gps.location.isValid()) {
            currentLat = gps.location.lat();
            currentLon = gps.location.lng();
            gpsValid = true;
            
            if (millis() - lastGpsPrint > 5000) {
                Serial.printf("GPS FIX: Lat: %.6f, Lon: %.6f\n", currentLat, currentLon);
                lastGpsPrint = millis();
            }
        } else {
            gpsValid = false;
        }
    }

    if (millis() > 5000 && gps.charsProcessed() < 10) {
        gpsValid = false;
    }
}

// ---------------------------------------------------------------------------
// Vraća je li GPS lokacija trenutno valjana
// ---------------------------------------------------------------------------
bool isGpsValid() { return gpsValid; }

// ---------------------------------------------------------------------------
// Dohvaća zadnju poznatu geografsku širinu (Latitude)
// ---------------------------------------------------------------------------
float getGpsLatitude() { return currentLat; }

// ---------------------------------------------------------------------------
// Dohvaća zadnju poznatu geografsku dužinu (Longitude)
// ---------------------------------------------------------------------------
float getGpsLongitude() { return currentLon; }