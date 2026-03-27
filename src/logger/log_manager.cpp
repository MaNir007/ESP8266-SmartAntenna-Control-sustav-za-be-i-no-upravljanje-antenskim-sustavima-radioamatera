/**
 * @file log_manager.cpp
 * @brief Implementacija sustava za upravljanje evidencijom radio veza (logovima).
 * @details Ovaj modul sadrži funkcije za dodavanje, brisanje i čitanje logova.
 * Logovi se čuvaju u lokalnoj JSON datoteci unutar LittleFS datotečnog sustava
 * radi otpornosti na gubitak napajanja, te kao radna kopija u standardnom vectoru.
 */

#include "log_manager.h"
#include "../config/config.h"
#include "../filesystem/fs_utils.h"
#include <ArduinoJson.h>

std::vector<LogEntry> radioLogs;

static void loadLogsFromLittleFS();
static void saveLogsToLittleFS();

/**
 * @brief Inicijalizira log manager učitavanjem postojećih logova iz LittleFS-a.
 */
void initLogManager() {
    loadLogsFromLittleFS();
}

/**
 * @brief Dodaje novi log u radnu memoriju i automatski ga sprema u trajnu memoriju.
 * @param newLog Objekt LogEntry koji sadrži sve podatke o radio vezi.
 */
void addLog(const LogEntry& newLog) {
    radioLogs.push_back(newLog);
    saveLogsToLittleFS();
}

/**
 * @brief Serijalizira sve logove u JSON niz znakova prikladan za slanje na frontend.
 * @return String koji predstavlja JSON array svih unosa.
 */
String getAllLogsAsJson() {
    JsonDocument doc; 
    JsonArray logsArray = doc.to<JsonArray>();

    for (const auto& log : radioLogs) {
        JsonObject logObj = logsArray.add<JsonObject>();
        logObj["time"] = log.time;
        logObj["callsign"] = log.callsign;
        logObj["country"] = log.country;
        logObj["name"] = log.name;
        logObj["frequency"] = log.frequency;
        logObj["modulation"] = log.modulation;
        logObj["rstSent"] = log.rstSent;
        logObj["rstReceived"] = log.rstReceived;
        logObj["qthlocator"] = log.qthlocator;
        logObj["notes"] = log.notes;
    }

    String jsonResponse;
    serializeJson(doc, jsonResponse);
    return jsonResponse;
}

/**
 * @brief Briše određeni log iz evidencije prema indeksu.
 * @param index Pozicija (0-indexed) loga unutar vektora.
 * @return True ako je brisanje uspješno (indeks valjan), inače false.
 */
bool deleteLog(int index) {
    if (index >= 0 && (size_t)index < radioLogs.size()) {
        radioLogs.erase(radioLogs.begin() + index);
        saveLogsToLittleFS();
        return true;
    }
    return false;
}

/**
 * @brief Uklanja apsolutno sve logove iz vektora i sprema prazan JSON fajl.
 * @return Uvijek vraća true.
 */
bool clearAllLogs() {
    radioLogs.clear();
    saveLogsToLittleFS();
    return true;
}

/**
 * @brief Pomoćna funkcija za inicijalno učitavanje logova iz JSON fajla.
 */
static void loadLogsFromLittleFS() {
    JsonDocument doc;
    if (!loadJsonFromFile(LOG_FILENAME, doc)) {
        radioLogs.clear();
        return;
    }
    JsonArray logsArray = doc.as<JsonArray>();
    radioLogs.clear();
    for (JsonObject logObj : logsArray) {
        LogEntry log;
        log.time = logObj["time"].as<String>();
        log.callsign = logObj["callsign"].as<String>();
        log.country = logObj["country"].as<String>();
        log.name = logObj["name"].as<String>();
        log.frequency = logObj["frequency"].as<float>();
        log.modulation = logObj["modulation"].as<String>();
        log.rstSent = logObj["rstSent"].as<String>();
        log.rstReceived = logObj["rstReceived"].as<String>();
        log.qthlocator = logObj["qthlocator"].as<String>();
        log.notes = logObj["notes"].as<String>();
        radioLogs.push_back(log);
    }
}

/**
 * @brief Pomoćna funkcija koja sprema trenutno stanje svih logova iz vektora u trajnu memoriju.
 */
static void saveLogsToLittleFS() {
    JsonDocument doc;
    JsonArray logsArray = doc.to<JsonArray>();
    for (const auto& log : radioLogs) {
        JsonObject logObj = logsArray.add<JsonObject>();
        logObj["time"] = log.time;
        logObj["callsign"] = log.callsign;
        logObj["country"] = log.country;
        logObj["name"] = log.name;
        logObj["frequency"] = log.frequency;
        logObj["modulation"] = log.modulation;
        logObj["rstSent"] = log.rstSent;
        logObj["rstReceived"] = log.rstReceived;
        logObj["qthlocator"] = log.qthlocator;
        logObj["notes"] = log.notes;
    }
    saveJsonToFile(LOG_FILENAME, doc);
}