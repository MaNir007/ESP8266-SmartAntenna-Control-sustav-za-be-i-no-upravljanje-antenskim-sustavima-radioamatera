// ---------------------------------------------------------------------------
// time_sync.h - Sučelje za sinkronizaciju vremena (NTP)
// ---------------------------------------------------------------------------
// Definira funkcije za inicijalizaciju NTP klijenta i dohvat vremena
// u formatiranom tekstualnom obliku.
// ---------------------------------------------------------------------------
#ifndef TIME_SYNC_H
#define TIME_SYNC_H

#include <Arduino.h>
#include <NTPClient.h> 
#include <WiFiUdp.h>   

extern NTPClient timeClient; 

void initTimeSync();
void updateTime(); 
String getFormattedUTCTime();

#endif // TIME_SYNC_H