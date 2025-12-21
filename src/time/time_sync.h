// src/time/time_sync.h
#ifndef TIME_SYNC_H
#define TIME_SYNC_H

#include <Arduino.h>
#include <NTPClient.h> // Potrebno za NTPClient objekt
#include <WiFiUdp.h>   // Potrebno za WiFiUDP objekt

// Deklaracija globalnog NTPClient objekta (definiran je u main.cpp)
extern NTPClient timeClient; // Važno: "extern" ovdje

void initTimeSync();
void updateTime(); // Funkcija za pozivanje u loop()
String getFormattedUTCTime();

#endif // TIME_SYNC_H