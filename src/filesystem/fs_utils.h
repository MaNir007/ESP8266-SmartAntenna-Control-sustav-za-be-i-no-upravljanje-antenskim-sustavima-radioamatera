// src/filesystem/fs_utils.h
#ifndef FS_UTILS_H
#define FS_UTILS_H

#include <Arduino.h>
#include <LittleFS.h>    // Potrebno za LittleFS klasu
#include <ArduinoJson.h> // Potrebno za JsonDocument

// Deklaracije funkcija za rad s datotečnim sustavom
void initLittleFS();
String getContentType(String filename);
bool handleFileRead(String path);
void listFS(); // Debug funkcija

// Općenite funkcije za učitavanje/spremanje JSON-a
bool loadJsonFromFile(const char* filename, JsonDocument& doc);
bool saveJsonToFile(const char* filename, const JsonDocument& doc);

// --- NOVE DEKLARACIJE ZA GRAFIKON I POZADINSKO SPREMANJE ---
// Ove funkcije omogućuju rad s log datotekom u pozadini
void appendLog(const char* path, String message);
String getLogContent(const char* path);
void clearLog(const char* path);

#endif // FS_UTILS_H