// ---------------------------------------------------------------------------
// wifi_manager.cpp - Povezivanje uređaja na WiFi mrežu
// ---------------------------------------------------------------------------
// Inicijalizira i održava vezu s WiFi pristupnom točkom.
// Sadrži logiku za spajanje, provjeru statusa i dohvat IP adrese.
// ---------------------------------------------------------------------------
#include "wifi_manager.h"
#include "../config/config.h" 



// ---------------------------------------------------------------------------
// Pokreće proceduru spajanja na WiFi
// ---------------------------------------------------------------------------
void initWiFi() {
    Serial.print("Povezivanje na WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD); 
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
    }
}

// ---------------------------------------------------------------------------
// Provjerava trenutni status veze
// ---------------------------------------------------------------------------
bool isWiFiConnected() {
    return WiFi.status() == WL_CONNECTED;
}

// ---------------------------------------------------------------------------
// Vraća lokalnu IP adresu uređaja
// ---------------------------------------------------------------------------
String getIPAddress() {
    return WiFi.localIP().toString();
}