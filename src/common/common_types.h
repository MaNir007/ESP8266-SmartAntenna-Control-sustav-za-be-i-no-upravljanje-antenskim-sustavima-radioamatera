#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <Arduino.h> // Potrebno za String klasu

struct LogEntry {
    String time;
    String callsign;
    String name;         // <--- DODANO OVO POLJE!
    float frequency;
    String modulation;
    String rstSent;      // OVO MORA BITI TU! (Već je bilo)
    String rstReceived;  // OVO MORA BITI TU! (Već je bilo)
    //String location;   // I dalje je zakomentirano/izbrisano kako je dogovoreno
    String qthlocator;
    String notes;
    // ... ostala polja
};

#endif