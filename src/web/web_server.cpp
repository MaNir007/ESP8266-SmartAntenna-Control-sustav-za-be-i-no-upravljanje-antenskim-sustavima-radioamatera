#include "web_server.h"
#include "../config/config.h"
#include "../filesystem/fs_utils.h"
#include "../sensors/sensor_manager.h"
#include "../antenna/antenna_control.h"
#include "../time/time_sync.h"
#include "../logger/log_manager.h"
#include "../common/common_types.h"
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <ESP8266WebServer.h>

extern ESP8266WebServer server;

// --- POMOĆNE FUNKCIJE ---

void handleRoot() {
    if (LittleFS.exists("/index.html")) {
        File file = LittleFS.open("/index.html", "r");
        server.streamFile(file, getContentType("/index.html"));
        file.close();
    } else {
        server.send(500, "text/plain", "Greska: index.html nije pronadjen!");
    }
}

void handleNotFound() {
    String path = server.uri();
    String contentType = getContentType(path);
    if (LittleFS.exists(path)) {
        File file = LittleFS.open(path, "r");
        server.streamFile(file, contentType);
        file.close();
        return;
    }
    server.send(404, "text/plain", "404 Nije pronadjeno: " + path);
}

// --- API ZA SENZORE I VRIJEME ---

void handleSensorReadings() {
    float temperature = getTemperature();
    float humidity = getHumidity();
    float voltage = getVoltage();

    JsonDocument doc; 
    if (temperature == -999.0 || humidity == -999.0) {
        doc["error"] = "Sensor read failed";
        String res;
        serializeJson(doc, res);
        server.send(500, "application/json", res);
        return;
    }
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["voltage"] = voltage;

    String res;
    serializeJson(doc, res);
    server.send(200, "application/json", res);
}

void handleUtcTime() {
    JsonDocument doc;
    doc["time"] = getFormattedUTCTime();
    String res;
    serializeJson(doc, res);
    server.send(200, "application/json", res);
}

// --- API ZA ANTENU I STEPER (Točni nazivi s GitHuba) ---

void handleAntennaStatus() {
    JsonDocument doc;
    doc["status"] = getAntennaType();
    String res;
    serializeJson(doc, res);
    server.send(200, "application/json", res);
}

void handleSetAntenna() {
    if (server.hasArg("plain")) {
        JsonDocument doc;
        deserializeJson(doc, server.arg("plain"));
        // Na GitHubu koristiš "type" kao ključ
        String type = doc["type"].as<String>();
        setAntennaType(type);
        server.send(200, "text/plain", "OK");
    }
}

void handleServoAngle() {
    JsonDocument doc;
    doc["angle"] = getCurrentServoAngle(); // Točan naziv s GitHuba
    String res;
    serializeJson(doc, res);
    server.send(200, "application/json", res);
}

void handleSetAngle() {
    if (server.hasArg("plain")) {
        JsonDocument doc;
        deserializeJson(doc, server.arg("plain"));
        int receivedAngle = doc["angle"].as<int>();
        setServoAngle(receivedAngle); // Točan naziv s GitHuba
        server.send(200, "text/plain", "OK");
    }
}

void handleCalibrateServo() {
    calibrateServo();
    server.send(200, "text/plain", "OK");
}

// --- NOVI LOG MANAGER API (Za logger.js) ---

void handleGetLogs() {
    server.send(200, "application/json", getAllLogsAsJson());
}

void handleSaveLog() {
    if (server.hasArg("plain")) {
        JsonDocument doc;
        deserializeJson(doc, server.arg("plain"));
        LogEntry entry;
        entry.time = doc["time"].as<String>();
        entry.callsign = doc["callsign"].as<String>();
        entry.name = doc["name"].as<String>();
        entry.frequency = doc["frequency"].as<float>();
        entry.modulation = doc["modulation"].as<String>();
        entry.rstSent = doc["rstSent"].as<String>();
        entry.rstReceived = doc["rstReceived"].as<String>();
        entry.qthlocator = doc["qthlocator"].as<String>();
        entry.notes = doc["notes"].as<String>();
        addLog(entry);
        server.send(200, "text/plain", "OK");
    }
}

void handleDeleteLog() {
    int index = server.arg("index").toInt();
    if (deleteLog(index)) server.send(200, "text/plain", "OK");
    else server.send(400, "text/plain", "Greska");
}

void handleClearAllLogs() {
    clearAllLogs();
    server.send(200, "text/plain", "OK");
}

// --- INICIJALIZACIJA ---

void initWebServer() {
    server.on("/", HTTP_GET, handleRoot);

    server.on("/logger", HTTP_GET, []() {
        if (LittleFS.exists("/logger.html")) {
            File file = LittleFS.open("/logger.html", "r");
            server.streamFile(file, "text/html");
            file.close();
        }
    });

    // API Rute
    server.on("/api/sensor_readings", HTTP_GET, handleSensorReadings);
    server.on("/api/antenna_status", HTTP_GET, handleAntennaStatus);
    server.on("/api/set_antenna", HTTP_POST, handleSetAntenna);
    server.on("/api/servo_angle", HTTP_GET, handleServoAngle);
    server.on("/api/set_angle", HTTP_POST, handleSetAngle);
    server.on("/api/calibrate_servo", HTTP_POST, handleCalibrateServo);
    server.on("/api/utc_time", HTTP_GET, handleUtcTime);

    // Logger Rute
    server.on("/logs", HTTP_GET, handleGetLogs);
    server.on("/save_log", HTTP_POST, handleSaveLog);
    server.on("/delete_log", HTTP_GET, handleDeleteLog);
    server.on("/clear_all_logs", HTTP_GET, handleClearAllLogs);

    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("Web server spreman!");
}

void handleClientRequests() {
    server.handleClient();
}