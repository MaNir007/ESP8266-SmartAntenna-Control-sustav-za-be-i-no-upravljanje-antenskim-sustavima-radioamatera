// src/filesystem/fs_utils.cpp
#include "fs_utils.h"
#include "../config/config.h" // Za LOG_FILENAME ili druge konstante

void initLittleFS() {
    if (!LittleFS.begin()) {
        Serial.println("Greška kod montiranja LittleFS-a! Pokušavam formatirati...");
        LittleFS.format(); // Pokušaj formatirati ako montiranje ne uspije
        if (!LittleFS.begin()) {
            Serial.println("Greška: LittleFS formatiranje i montiranje neuspješno!");
            // Ovdje možete dodati fatalnu grešku ili reboot
            return;
        }
    }
    Serial.println("LittleFS uspješno montiran.");
    listFS(); // Ispiši sadržaj LittleFS-a za debug
}

String getContentType(String filename) {
    if (filename.endsWith(".html")) return "text/html";
    if (filename.endsWith(".css")) return "text/css";
    if (filename.endsWith(".js")) return "application/javascript";
    if (filename.endsWith(".json")) return "application/json";
    if (filename.endsWith(".ico")) return "image/x-icon";
    if (filename.endsWith(".svg")) return "image/svg+xml";
    if (filename.endsWith(".png")) return "image/png";
    if (filename.endsWith(".jpg")) return "image/jpeg";
    if (filename.endsWith(".gif")) return "image/gif";
    if (filename.endsWith(".pdf")) return "application/pdf";
    if (filename.endsWith(".zip")) return "application/zip";
    if (filename.endsWith(".gz")) return "application/x-gzip";
    // Dodani Font Awesome tipovi
    if (filename.endsWith(".woff")) return "font/woff";
    if (filename.endsWith(".woff2")) return "font/woff2";
    if (filename.endsWith(".ttf")) return "font/ttf";
    return "text/plain"; // Zadani tip za nepoznate ekstenzije
}

bool handleFileRead(String path) {
    Serial.println("fs_utils::handleFileRead: Pokušavam otvoriti: " + path);
    if (path.endsWith("/")) path += "index.html"; // Ako je ruta direktorij, pokušaj poslužiti index.html

    File file = LittleFS.open(path, "r");
    if (!file) {
        Serial.println("Greška: Datoteka nije pronađena: " + path);
        return false;
    }

    Serial.println("Uspješno otvoreno: " + path + " (Veličina: " + String(file.size()) + " bajta)");
    file.close();
    return true;
}


void listFS() {
    Serial.println("\n--- Sadržaj LittleFS-a ---");
    Dir dir = LittleFS.openDir("/");
    while (dir.next()) {
        Serial.print(" - " + dir.fileName());
        File f = dir.openFile("r");
        if (f) {
            Serial.println(" (" + String(f.size()) + " bajta)");
            f.close();
        } else {
            Serial.println(" (Greška pri otvaranju)");
        }
    }
    Serial.println("---------------------------");
}

// Općenita funkcija za učitavanje JSON-a iz datoteke
bool loadJsonFromFile(const char* filename, JsonDocument& doc) {
    File file = LittleFS.open(filename, "r");
    if (!file) {
        Serial.println("Greška: Datoteka " + String(filename) + " nije pronađena ili se ne može otvoriti.");
        return false;
    }

    size_t fileSize = file.size();
    if (fileSize == 0) {
        Serial.println("Datoteka " + String(filename) + " je prazna.");
        file.close();
        return false;
    }

    // Pročitaj cijeli sadržaj datoteke u string
    String fileContent = file.readString();
    file.close();

    // Pokušaj deserializirati JSON
    DeserializationError error = deserializeJson(doc, fileContent);
    if (error) {
        Serial.print("deserializeJson() neuspješan za " + String(filename) + ": ");
        Serial.println(error.c_str());
        // Uklonjen doc.memoryUsage() jer je deprecated
        Serial.println("Greška: Provjerite veličinu JsonDocument buffera (ili neka se alocira dinamički).");
        return false;
    }
    return true;
}

// Općenita funkcija za spremanje JSON-a u datoteku
bool saveJsonToFile(const char* filename, const JsonDocument& doc) {
    File file = LittleFS.open(filename, "w");
    if (!file) {
        Serial.println("Greška pri otvaranju " + String(filename) + " za pisanje.");
        return false;
    }

    // Uklonjen 'doc.capacity()' i 'doc.memoryUsage()'
    if (serializeJson(doc, file) == 0) {
        Serial.println("Greška pri pisanju JSON-a u " + String(filename) + ".");
        // Ako koristite JsonDocument bez eksplicitne veličine (dinamička alokacija),
        // ne možete direktno provjeriti 'capacity()' ili 'memoryUsage()' na taj način.
        // Greška "serializeJson() == 0" je dovoljna indikacija.
        Serial.println("Provjerite da li je JsonDocument uspio alocirati dovoljno memorije.");
        file.close();
        return false;
    }
    file.close();
    return true;
}