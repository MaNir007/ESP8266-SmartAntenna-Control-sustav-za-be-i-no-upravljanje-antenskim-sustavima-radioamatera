#include "sensor_manager.h"
#include "../config/config.h" 
#include <DHT.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// --- EXTERN DEKLARACIJE ---
// Povezujemo se s objektima koji su definirani u main.cpp
extern DHT dht;
extern SoftwareSerial gpsSerial;
extern TinyGPSPlus gps;
extern float currentLat;
extern float currentLon;
extern bool gpsValid;

/**
 * @brief Inicijalizira sve senzore sustava.
 */
void initSensors() {
    dht.begin();
    Serial.println("DHT22 senzor inicijaliziran.");
    
    initGps(); 
}

/**
 * @brief Očitava temperaturu s DHT senzora.
 */
float getTemperature() {
    float temperature = dht.readTemperature();
    if (isnan(temperature)) {
        Serial.println("Greška: DHT očitavanje (Temp) neuspješno!");
        return -999.0;
    }
    return temperature;
}

/**
 * @brief Očitava vlažnost s DHT senzora.
 */
float getHumidity() {
    float humidity = dht.readHumidity();
    if (isnan(humidity)) {
        Serial.println("Greška: DHT očitavanje (Hum) neuspješno!");
        return -999.0;
    }
    return humidity;
}

/**
 * @brief Izračunava napon sustava.
 * Ako imaš naponski djelitelj na A0 (npr. 10k i 2.2k), ovdje ide formula.
 */
float getVoltage() {
    // ADC na ESP8266 ide do 1.0V (interni) ili 3.3V (na NodeMCU pločama)
    // Primjer za 3.3V sustav bez vanjskog djelitelja:
    int raw = analogRead(A0);
    return (raw / 1023.0) * 3.3; 
}

// --- GPS FUNKCIJE ---

void initGps() {
    // GPS_BAUD_RATE bi trebao biti u config.h (najčešće 9600 za Neo-6M)
    #ifndef GPS_BAUD_RATE
        #define GPS_BAUD_RATE 9600
    #endif

    gpsSerial.begin(GPS_BAUD_RATE);
    Serial.println("GPS SoftwareSerial pokrenut na 9600 baud.");
}

void updateGpsData() {
    // Čitanje podataka s GPS serijskog porta
    while (gpsSerial.available()) {
        gps.encode(gpsSerial.read());
    }

    // Provjera je li lokacija ažurirana
    if (gps.location.isUpdated()) {
        if (gps.location.isValid()) {
            currentLat = gps.location.lat();
            currentLon = gps.location.lng();
            gpsValid = true;
            
            // Ispis svakih par sekundi da ne zatrpavamo Serial monitor
            static unsigned long lastGpsPrint = 0;
            if (millis() - lastGpsPrint > 5000) {
                Serial.printf("GPS FIX: Lat: %.6f, Lon: %.6f\n", currentLat, currentLon);
                lastGpsPrint = millis();
            }
        } else {
            gpsValid = false;
        }
    }

    // Sigurnosni timeout: ako nema znakova duže vrijeme, GPS je vjerojatno odspojen
    if (millis() > 5000 && gps.charsProcessed() < 10) {
        gpsValid = false;
    }
}

bool isGpsValid() { return gpsValid; }
float getGpsLatitude() { return currentLat; }
float getGpsLongitude() { return currentLon; }