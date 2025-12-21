#include "log_manager.h"
#include "../config/config.h"
#include "../filesystem/fs_utils.h"
#include <ArduinoJson.h>

std::vector<LogEntry> radioLogs;

static void loadLogsFromLittleFS();
static void saveLogsToLittleFS();

void initLogManager() {
    loadLogsFromLittleFS();
}

void addLog(const LogEntry& newLog) {
    radioLogs.push_back(newLog);
    saveLogsToLittleFS();
}

String getAllLogsAsJson() {
    JsonDocument doc; 
    JsonArray logsArray = doc.to<JsonArray>();

    for (const auto& log : radioLogs) {
        JsonObject logObj = logsArray.add<JsonObject>();
        logObj["time"] = log.time;
        logObj["callsign"] = log.callsign;
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

bool deleteLog(int index) {
    if (index >= 0 && (size_t)index < radioLogs.size()) {
        radioLogs.erase(radioLogs.begin() + index);
        saveLogsToLittleFS();
        return true;
    }
    return false;
}

bool clearAllLogs() {
    radioLogs.clear();
    saveLogsToLittleFS();
    return true;
}

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

static void saveLogsToLittleFS() {
    JsonDocument doc;
    JsonArray logsArray = doc.to<JsonArray>();
    for (const auto& log : radioLogs) {
        JsonObject logObj = logsArray.add<JsonObject>();
        logObj["time"] = log.time;
        logObj["callsign"] = log.callsign;
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