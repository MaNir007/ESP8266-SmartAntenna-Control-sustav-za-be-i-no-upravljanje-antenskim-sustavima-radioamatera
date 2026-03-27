 
/**
 * @file wifi_manager.cpp
 * @brief Povezivanje uređaja u lokalnu bežičnu mrežu.
 * @details Inicijalizira i održava vezu s WiFi pristupnom točkom definiranom u config.h.
 */
// src/wifi/wifi_manager.cpp
#include "wifi_manager.h"
#include "../config/config.h" // Pristup konfiguraciji

// Instanca WiFiClient (ako se koristi, npr. za MQTT)
// WiFiClient espClient;

/**
 * @brief Pokreće proceduru spajanja na WiFi s definiranim kredencijalima.
 * U slučaju uzastopnih neuspješnih pokušaja, ispisuje grešku (moguć soft-AP fallback).
 */
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

/**
 * @brief Provjerava trenutni status veze.
 * @return True ako je uređaj uspješno povezan s mrežom, inače false.
 */
bool isWiFiConnected() {
    return WiFi.status() == WL_CONNECTED;
}

/**
 * @brief Vraća lokalnu IP adresu uređaja u obliku Stringa.
 */
String getIPAddress() {
    return WiFi.localIP().toString();
}