// ---------------------------------------------------------------------------
// common_types.h - Zajedničke strukture podataka sustava
// ---------------------------------------------------------------------------
// Sadrži definicije struktura koje se koriste u više modula, 
// poput LogEntry za spremanje podataka o radio vezama.
// ---------------------------------------------------------------------------
#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <Arduino.h> 

struct LogEntry {
    String time;
    String callsign;
    String country;
    String name;
    float frequency;
    String modulation;
    String rstSent;
    String rstReceived;
    String qthlocator;
    String sentNr;
    String rcvdNr;
    String notes;

};

#endif