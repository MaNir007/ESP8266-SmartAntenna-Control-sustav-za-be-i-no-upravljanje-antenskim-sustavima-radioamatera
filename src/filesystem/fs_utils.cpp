/**
 * @file fs_utils.cpp
 * @brief Operacije s LittleFS datotečnim sustavom.
 * @details Modul omogućava montiranje FS-a, čitanje i pisanje datoteka te 
 * pomoćne funkcije za rad sa JSON-om i CSV logovima senzora.
 */
// src/filesystem/fs_utils.cpp
#include "fs_utils.h"
#include "../config/config.h" // Za LOG_FILENAME ili druge konstante

/**
 * @brief Montira (pokreće) LittleFS na ESP8266.
 * Ako inicijalizacija ne uspije (npr. zbog oštećenog flasha), 
 * funkcija ga pokušava ponovno formatirati.
 */
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

/**
 * @brief Vraća odgovarajući MIME tip za posluživanje datoteka putem HTTP-a na temelju ekstenzije.
 * @param filename Putanja ili naziv datoteke.
 * @return String s nazivom MIME tipa, npr. "text/html" ili "application/json".
 */
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

/**
 * @brief Otvara i vrši provjeru postojanja datoteke unutar sustava prije slanja na web server.
 * @param path Putanja do datoteke, sa ili bez kosih crta na kraju.
 * @return True ako je otvaranje uspjelo, false inače.
 */
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


/**
 * @brief Popisuje sve datoteke unutar LittleFS korijenskog direktorija u Serial Monitor (za debug svrhe).
 */
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

/**
 * @brief Općenita funkcija za učitavanje i parsiranje JSON datoteke s diska pomoću ArduinoJson modula.
 * @param filename Naziv Json datoteke koju učitavamo.
 * @param doc Referenca na JSON objekt u kojeg će biti parsirani podaci.
 * @return True ako je učitavanje i deserializacija JSON datoteke uspješna.
 */
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

/**
 * @brief Općenita funkcija za spremanje (serijaliziranje) JSON dokumenta u tekstualnu datoteku.
 * @param filename Naziv (putanja) datoteke. Uvijek prebriše postojeću verziju.
 * @param doc Referenca na ArduinoJson dokument čiji se sadržaj serijalizira.
 * @return True ako se datoteka uspjela otvoriti te dokument serijalizirati, a False inače.
 */
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

// --- NOVE FUNKCIJE ZA GRAFIKON I POZADINSKO SPREMANJE ---

/**
 * @brief Funkcija za dodavanje jedne linije teksta (zapisa) na kraj postojeće datoteke.
 * @details Koristi mod "a" (append) čime se izbjegava čitanje cijele datoteke. 
 * Također implementira svojevrsnu "log rotaciju", kojom automatski prazni
 * datoteku ako log naraste preko zadane maksimalne sigurnosne veličine 100KB.
 * Time se izuzetno poboljšava otpornost koda ("slabe točke" spriječene).
 * @param path Lokacija csv log datoteke, npr. /data_log.csv
 * @param message Zapis oblika teksta koji se dodaje, idealno JSON redak ili CSV.
 */
void appendLog(const char* path, String message) {
    File file = LittleFS.open(path, "a"); // "a" znači append (dodaj na kraj)
    if (!file) {
        Serial.println("Greška pri otvaranju log datoteke!");
        return;
    }
    file.println(message);
    file.close();

    // Sigurnosna provjera veličine: ako log prijeđe 100KB, obriši ga da ne prepuni memoriju
    File check = LittleFS.open(path, "r");
    if (check && check.size() > 102400) {
        check.close();
        LittleFS.remove(path);
        Serial.println("Log datoteka resetirana (prešla 100KB).");
    }
}

/**
 * @brief Funkcija koja u cijelosti čita tekstualnu datoteku (npr. CSV senzor log).
 * @param path Lokacija csv/txt datoteke.
 * @return Cijela povijest loga (String). Vraća prazan znak ako datoteka ne postoji.
 */
String getLogContent(const char* path) {
    File file = LittleFS.open(path, "r");
    if (!file) return "";
    String content = file.readString();
    file.close();
    return content;
}

/**
 * @brief Potpuno briše naznačenu datoteku unutar FS-a. Koristi se za brisanje graf povijesti.
 * @param path Datoteka koju želimo obrisati.
 */
void clearLog(const char* path) {
    if (LittleFS.exists(path)) {
        LittleFS.remove(path);
        Serial.println("Log datoteka obrisana.");
    }
}