// src/time/time_sync.cpp
#include "time_sync.h"
#include "../config/config.h" // Potrebno za NTP_SERVER, NTP_OFFSET, NTP_UPDATE_INTERVAL
#include <time.h>            // Potrebno za struct tm i gmtime_r
#include <Arduino.h>         // Potrebno za Serial.print/println i String

// Globalni objekti su deklarirani kao 'extern' u .h i definirani u main.cpp
// Ovdje ih samo koristimo.

void initTimeSync() {
    timeClient.begin(); // Pokreće NTP klijenta
    Serial.print("Sinkronizacija vremena s NTP serverom...");
    // timeClient.forceUpdate() prisiljava ažuriranje vremena, što je dobro za inicijalizaciju.
    if (timeClient.forceUpdate()) {
        Serial.println("Uspješno!");
        // timeClient.getFormattedTime() vraća vrijeme formatirano kao HH:MM:SS
        Serial.println("Početno UTC vrijeme (iz NTP-a): " + timeClient.getFormattedTime());
        Serial.print("Početno epoch vrijeme (iz NTP-a): ");
        Serial.println(timeClient.getEpochTime());
    } else {
        Serial.println("Neuspješno. Vrijeme možda nije točno. Provjerite mrežnu vezu ili NTP server.");
        // Ispišimo epoch vrijeme i nakon neuspjeha da vidimo što dobije:
        Serial.print("Pokušaj dohvaćanja epochTime nakon inicijalnog neuspjeha: ");
        Serial.println(timeClient.getEpochTime());
    }
}

void updateTime() {
    // Ova funkcija se poziva iz getFormattedUTCTime() kako bi osigurala
    // da imamo najnovije vrijeme prije formatiranja.
    // timeClient.update() ne blokira, i vraća true ako je uspješno ažurirao.
    if (!timeClient.update()) {
        // Ova poruka će se pojavljivati redovito ako NTP server nije dostupan,
        // ali ne smije blokirati izvršavanje programa.
        // Bit će zabilježena u serijskom monitoru.
        Serial.println("NTP klijent nije uspio ažurirati vrijeme.");
    }
}

String getFormattedUTCTime() {
    updateTime(); // Prvo pokušaj ažurirati vrijeme

    unsigned long currentEpochTimeUL = timeClient.getEpochTime(); // Dohvati epoch vrijeme kao unsigned long

    // --- DEBUG ISPISI ---
    Serial.print("Debug [getFormattedUTCTime]: Dohvaćena epochTime (unsigned long) = ");
    Serial.println(currentEpochTimeUL);
    // --- KRAJ DEBUG ISPISA ---

    // Provjera valjanosti epoch vremena:
    // Ako je epochTime 0 ili vrlo mala (npr. prije 1. siječnja 2000. godine),
    // to obično znači da vrijeme nije ispravno sinkronizirano.
    // 946684800UL je epoch time za 2000-01-01 00:00:00 UTC
    if (currentEpochTimeUL < 946684800UL) {
        Serial.println("Upozorenje [getFormattedUTCTime]: Dohvaćena epochTime je preniska (< 2000. godine). Vrijeme nije sinkronizirano ili je nevažeće.");
        return "N/A - Vrijeme nije sinkronizirano";
    }

    // KONVERZIJA unsigned long U time_t
    // Na ESP8266, 'time_t' je obično definiran kao 'long' (signed 32-bitni integer).
    // Maksimalna pozitivna vrijednost za 32-bitni signed integer je 2,147,483,647.
    // Epoch vrijeme za 2038-01-19 03:14:07 UTC je 2,147,483,647.
    // Ako 'currentEpochTimeUL' pređe tu vrijednost, doći će do "Year 2038 problem"
    // gdje će se broj prebaciti u negativnu vrijednost pri kastiranju u 'signed long'.
    // To bi rezultiralo datumom poput -2988138-06-09.

    if (currentEpochTimeUL > 2147483647UL) {
        Serial.println("Upozorenje [getFormattedUTCTime]: epochTime je prevelika za signed time_t (mogući 'Year 2038 problem').");
        // Ako se ovo dogodi prije 2038, znači da je negdje u NTPClientu ili sustavu
        // greška i da vraća preveliku vrijednost.
        // Za sada, vrati poruku o grešci, kasnije se može razmisliti o alternativnom formatiranju.
        return "N/A - Prevelika epoch time (2038 problem)";
    }

    // Ako je sve u redu, pretvori unsigned long u time_t
    time_t rawtime = (time_t)currentEpochTimeUL;

    // --- DEBUG ISPISI ---
    Serial.print("Debug [getFormattedUTCTime]: Konvertirana rawtime (time_t/signed long) = ");
    Serial.println(rawtime);
    // --- KRAJ DEBUG ISPISA ---

    struct tm current_tm;
    // Koristimo gmtime_r jer je sigurniji (reentrant) za okruženja poput AsyncWebServera.
    // gmtime_r zahtijeva pokazivač na time_t i pokazivač na tm strukturu.
    if (gmtime_r(&rawtime, &current_tm) == nullptr) {
        Serial.println("Greška [getFormattedUTCTime]: gmtime_r funkcija neuspješna.");
        return "N/A - Greška formatiranja vremena";
    }

    char formattedDateTime[33]; // YYYY-MM-DD HH:MM:SS je 19 znakova.
                                // Dodajemo mjesta za null terminator (\0) i malo lufta za sigurnost.
                                // 19 + 1 = 20. 33 je više nego dovoljno.

    // snprintf je sigurna funkcija jer sprječava buffer overflow.
    // Formatiramo vrijeme u željeni string format.
    snprintf(formattedDateTime, sizeof(formattedDateTime), "%04d-%02d-%02d %02d:%02d:%02d",
             current_tm.tm_year + 1900, // tm_year je broj godina od 1900
             current_tm.tm_mon + 1,     // tm_mon je 0-11
             current_tm.tm_mday,        // tm_mday je 1-31
             current_tm.tm_hour,        // tm_hour je 0-23
             current_tm.tm_min,         // tm_min je 0-59
             current_tm.tm_sec);        // tm_sec je 0-59

    return String(formattedDateTime);
}