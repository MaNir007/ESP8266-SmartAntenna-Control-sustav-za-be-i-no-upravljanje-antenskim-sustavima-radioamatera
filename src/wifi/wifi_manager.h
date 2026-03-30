// ---------------------------------------------------------------------------
// wifi_manager.h - Sučelje za upravljanje bežičnom vezom
// ---------------------------------------------------------------------------
// Definira funkcije za povezivanje uređaja na mrežu, provjeru statusa
// i dohvaćanje mrežnih informacija poput IP adrese.
// ---------------------------------------------------------------------------
#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <ESP8266WiFi.h> 

// ---------------------------------------------------------------------------
// Deklaracija WiFiClient objekta ako bi bio potreban
// ---------------------------------------------------------------------------
void initWiFi();
bool isWiFiConnected();
String getIPAddress();

#endif // WIFI_MANAGER_H