// ---------------------------------------------------------------------------
// fs_utils.cpp - Operacije s LittleFS datotečnim sustavom
// ---------------------------------------------------------------------------
// Modul omogućava montiranje FS-a, čitanje i pisanje datoteka te 
// pomoćne funkcije za rad sa JSON-om i CSV logovima senzora.
// ---------------------------------------------------------------------------
#include "fs_utils.h"
#include "../config/config.h" // Za LOG_FILENAME ili druge konstante

// ---------------------------------------------------------------------------
// Montira (pokreće) LittleFS na ESP8266
// ---------------------------------------------------------------------------
void initLittleFS() {
    if (!LittleFS.begin()) {
        Serial.println("Greška kod montiranja LittleFS-a! Pokušavam formatirati...");
        LittleFS.format(); 
        if (!LittleFS.begin()) {
            Serial.println("Greška: LittleFS formatiranje i montiranje neuspješno!");
            return;
        }
    }
    Serial.println("LittleFS uspješno montiran.");
    listFS(); 
}

// ---------------------------------------------------------------------------
// Vraća odgovarajući MIME tip za posluživanje datoteka
// ---------------------------------------------------------------------------
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
    return "text/plain"; 
}

// ---------------------------------------------------------------------------
// Provjera postojanja datoteke prije slanja na web server
// ---------------------------------------------------------------------------
bool handleFileRead(String path) {
    Serial.println("fs_utils::handleFileRead: Pokušavam otvoriti: " + path);
    if (path.endsWith("/")) path += "index.html"; 

    File file = LittleFS.open(path, "r");
    if (!file) {
        Serial.println("Greška: Datoteka nije pronađena: " + path);
        return false;
    }

    Serial.println("Uspješno otvoreno: " + path + " (Veličina: " + String(file.size()) + " bajta)");
    file.close();
    return true;
}


// ---------------------------------------------------------------------------
// Popisuje sve datoteke unutar LittleFS korijenskog direktorija
// ---------------------------------------------------------------------------
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

// ---------------------------------------------------------------------------
// Učitavanje i parsiranje JSON datoteke s diska
// ---------------------------------------------------------------------------
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

    String fileContent = file.readString();
    file.close();

    DeserializationError error = deserializeJson(doc, fileContent);
    if (error) {
        Serial.print("deserializeJson() neuspješan za " + String(filename) + ": ");
        Serial.println(error.c_str());
        Serial.println("Greška: Provjerite veličinu JsonDocument buffera (ili neka se alocira dinamički).");
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Spremanje (serijaliziranje) JSON dokumenta u tekstualnu datoteku
// ---------------------------------------------------------------------------
bool saveJsonToFile(const char* filename, const JsonDocument& doc) {
    File file = LittleFS.open(filename, "w");
    if (!file) {
        Serial.println("Greška pri otvaranju " + String(filename) + " za pisanje.");
        return false;
    }

    if (serializeJson(doc, file) == 0) {
        Serial.println("Greška pri pisanju JSON-a u " + String(filename) + ".");
        Serial.println("Provjerite da li je JsonDocument uspio alocirati dovoljno memorije.");
        file.close();
        return false;
    }
    file.close();
    return true;
}

// ---------------------------------------------------------------------------
// NOVE FUNKCIJE ZA GRAFIKON I POZADINSKO SPREMANJE
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Funkcija za dodavanje jedne linije teksta na kraj postojeće datoteke
// ---------------------------------------------------------------------------
void appendLog(const char* path, String message) {
    File file = LittleFS.open(path, "a"); 
    if (!file) {
        Serial.println("Greška pri otvaranju log datoteke!");
        return;
    }
    file.println(message);
    file.close();

    File check = LittleFS.open(path, "r");
    if (check && check.size() > 102400) {
        check.close();
        LittleFS.remove(path);
        Serial.println("Log datoteka resetirana (prešla 100KB).");
    }
}

// ---------------------------------------------------------------------------
// Čita tekstualnu datoteku u cijelosti
// ---------------------------------------------------------------------------
String getLogContent(const char* path) {
    File file = LittleFS.open(path, "r");
    if (!file) return "";
    String content = file.readString();
    file.close();
    return content;
}

// ---------------------------------------------------------------------------
// Potpuno briše naznačenu datoteku unutar FS-a
// ---------------------------------------------------------------------------
void clearLog(const char* path) {
    if (LittleFS.exists(path)) {
        LittleFS.remove(path);
        Serial.println("Log datoteka obrisana.");
    }
}