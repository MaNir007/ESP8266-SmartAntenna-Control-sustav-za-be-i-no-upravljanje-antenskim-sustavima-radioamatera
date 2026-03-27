#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <Arduino.h> // Potrebno za String klasu

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
    String notes;
    // ... ostala polja
};

#endif