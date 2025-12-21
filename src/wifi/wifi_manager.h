 
// src/wifi/wifi_manager.h
#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <ESP8266WiFi.h> // Potrebno za WiFi klasu

// Deklaracija WiFiClient objekta ako bi bio potreban za npr. MQTT
// extern WiFiClient espClient;

void initWiFi();
bool isWiFiConnected();
String getIPAddress();

#endif // WIFI_MANAGER_H