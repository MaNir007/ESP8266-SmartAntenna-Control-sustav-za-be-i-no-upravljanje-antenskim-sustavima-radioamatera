// ---------------------------------------------------------------------------
// log_manager.cpp - Upravljanje evidencijom radio veza (logovima)
// ---------------------------------------------------------------------------
// Modul sadrži funkcije za dodavanje, brisanje i čitanje logova.
// Logovi se čuvaju u lokalnoj JSON datoteci unutar LittleFS sustava
// te kao radna kopija u standardnom vectoru radi bržeg pristupa.
// ---------------------------------------------------------------------------

#include "log_manager.h"
#include "../config/config.h"
#include "../filesystem/fs_utils.h"
#include <ArduinoJson.h>

std::vector<LogEntry> radioLogs;

static void loadLogsFromLittleFS();
static void saveLogsToLittleFS();

// ---------------------------------------------------------------------------
// Inicijalizira log manager učitavanjem logova iz LittleFS-a
// ---------------------------------------------------------------------------
void initLogManager() {
    loadLogsFromLittleFS();
}

// ---------------------------------------------------------------------------
// Dodaje novi log u radnu memoriju i automatski ga sprema
// ---------------------------------------------------------------------------
void addLog(const LogEntry& newLog) {
    radioLogs.push_back(newLog);
    saveLogsToLittleFS();
}

// ---------------------------------------------------------------------------
// Serijalizira sve logove u JSON niz znakova
// ---------------------------------------------------------------------------
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
        logObj["sentNr"] = log.sentNr;
        logObj["rcvdNr"] = log.rcvdNr;
        logObj["notes"] = log.notes;
    }

    String jsonResponse;
    serializeJson(doc, jsonResponse);
    return jsonResponse;
}

// ---------------------------------------------------------------------------
// Briše određeni log iz evidencije prema indeksu
// ---------------------------------------------------------------------------
bool deleteLog(int index) {
    if (index >= 0 && (size_t)index < radioLogs.size()) {
        radioLogs.erase(radioLogs.begin() + index);
        saveLogsToLittleFS();
        return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// Uklanja apsolutno sve logove iz vektora
// ---------------------------------------------------------------------------
bool clearAllLogs() {
    radioLogs.clear();
    saveLogsToLittleFS();
    return true;
}

// ---------------------------------------------------------------------------
// Pomoćna funkcija za inicijalno učitavanje logova iz JSON fajla
// ---------------------------------------------------------------------------
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
        log.sentNr = logObj["sentNr"].as<String>();
        log.rcvdNr = logObj["rcvdNr"].as<String>();
        log.notes = logObj["notes"].as<String>();
        radioLogs.push_back(log);
    }
}

// ---------------------------------------------------------------------------
// Sprema trenutno stanje svih logova u trajnu memoriju
// ---------------------------------------------------------------------------
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
        logObj["sentNr"] = log.sentNr;
        logObj["rcvdNr"] = log.rcvdNr;
        logObj["notes"] = log.notes;
    }
    saveJsonToFile(LOG_FILENAME, doc);
}