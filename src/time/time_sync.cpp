// ---------------------------------------------------------------------------
// time_sync.cpp - Sinkronizacija točnog vremena putem NTP protokola
// ---------------------------------------------------------------------------
// Dohvaća vrijeme s internetskog NTP servera i pretvara ga iz
// epoch timestampa u prikladni YYYY-MM-DD HH:MM:SS format za logiranje.
// ---------------------------------------------------------------------------
#include "time_sync.h"
#include "../config/config.h" 
#include <time.h>            
#include <Arduino.h>         



// ---------------------------------------------------------------------------
// Inicijalizira NTP klijenta i vrši početnu sinkronizaciju
// ---------------------------------------------------------------------------
void initTimeSync() {
    timeClient.begin(); 
    Serial.print("Sinkronizacija vremena s NTP serverom...");
    if (timeClient.forceUpdate()) {
        Serial.println("Uspješno!");
        Serial.println("Početno UTC vrijeme (iz NTP-a): " + timeClient.getFormattedTime());
        Serial.print("Početno epoch vrijeme (iz NTP-a): ");
        Serial.println(timeClient.getEpochTime());
    } else {
        Serial.println("Neuspješno. Vrijeme možda nije točno. Provjerite mrežnu vezu ili NTP server.");
        Serial.print("Pokušaj dohvaćanja epochTime nakon inicijalnog neuspjeha: ");
        Serial.println(timeClient.getEpochTime());
    }
}

// ---------------------------------------------------------------------------
// Periodički osvježava vrijeme u pozadini
// ---------------------------------------------------------------------------
void updateTime() {
    if (!timeClient.update()) {
        Serial.println("NTP klijent nije uspio ažurirati vrijeme.");
    }
}

// ---------------------------------------------------------------------------
// Dohvaća trenutno UTC vrijeme formatirano kao niz znakova
// ---------------------------------------------------------------------------
String getFormattedUTCTime() {
    updateTime(); 

    unsigned long currentEpochTimeUL = timeClient.getEpochTime(); 



    if (currentEpochTimeUL < 946684800UL) {
        Serial.println("Upozorenje [getFormattedUTCTime]: Dohvaćena epochTime je preniska (< 2000. godine). Vrijeme nije sinkronizirano ili je nevažeće.");
        return "N/A - Vrijeme nije sinkronizirano";
    }



    if (currentEpochTimeUL > 2147483647UL) {
        Serial.println("Upozorenje [getFormattedUTCTime]: epochTime je prevelika za signed time_t (mogući 'Year 2038 problem').");
        return "N/A - Prevelika epoch time (2038 problem)";
    }

    time_t rawtime = (time_t)currentEpochTimeUL;



    struct tm current_tm;
    if (gmtime_r(&rawtime, &current_tm) == nullptr) {
        Serial.println("Greška [getFormattedUTCTime]: gmtime_r funkcija neuspješna.");
        return "N/A - Greška formatiranja vremena";
    }

    char formattedDateTime[64]; 


    snprintf(formattedDateTime, sizeof(formattedDateTime), "%04d-%02d-%02d %02d:%02d:%02d",
             current_tm.tm_year + 1900, 
             current_tm.tm_mon + 1,     
             current_tm.tm_mday,        
             current_tm.tm_hour,        
             current_tm.tm_min,         
             current_tm.tm_sec);        

    return String(formattedDateTime);
}