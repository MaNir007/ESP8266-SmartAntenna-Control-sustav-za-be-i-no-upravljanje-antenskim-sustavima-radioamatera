 
// src/wifi/wifi_manager.cpp
#include "wifi_manager.h"
#include "../config/config.h" // Pristup konfiguraciji

// Instanca WiFiClient (ako se koristi, npr. za MQTT)
// WiFiClient espClient;

void initWiFi() {
    Serial.print("Povezivanje na WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Koristimo konstante iz config.h
    int connectAttempts = 0;
    while (WiFi.status() != WL_CONNECTED && connectAttempts < 20) {
        delay(500);
        Serial.print(".");
        connectAttempts++;
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nPovezan! IP adresa: " + WiFi.localIP().toString());
    } else {
        Serial.println("\nWiFi povezivanje neuspješno nakon " + String(connectAttempts) + " pokušaja!");
        Serial.println("Provjerite SSID/lozinku i pokušajte ponovo.");
        // Ovdje možete dodati logiku za pad back na AP način rada ako je potrebno
    }
}

bool isWiFiConnected() {
    return WiFi.status() == WL_CONNECTED;
}

String getIPAddress() {
    return WiFi.localIP().toString();
}