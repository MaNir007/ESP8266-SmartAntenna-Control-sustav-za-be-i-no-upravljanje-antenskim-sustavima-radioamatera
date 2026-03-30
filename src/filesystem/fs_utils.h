// ---------------------------------------------------------------------------
// fs_utils.h - Sučelje za rad s datotečnim sustavom (LittleFS)
// ---------------------------------------------------------------------------
// Sadrži deklaracije funkcija za montiranje sustava, čitanje datoteka,
// određivanje MIME tipova i manipulaciju JSON/CSV logovima.
// ---------------------------------------------------------------------------
#ifndef FS_UTILS_H
#define FS_UTILS_H

#include <Arduino.h>
#include <LittleFS.h>    
#include <ArduinoJson.h> 


void initLittleFS();
String getContentType(String filename);
bool handleFileRead(String path);
void listFS(); 


bool loadJsonFromFile(const char* filename, JsonDocument& doc);
bool saveJsonToFile(const char* filename, const JsonDocument& doc);


void appendLog(const char* path, String message);
String getLogContent(const char* path);
void clearLog(const char* path);

#endif // FS_UTILS_H