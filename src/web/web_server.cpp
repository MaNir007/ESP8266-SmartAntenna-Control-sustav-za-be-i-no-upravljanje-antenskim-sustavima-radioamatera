// ---------------------------------------------------------------------------
// web_server.cpp - Web Server & REST API Implementation
// ---------------------------------------------------------------------------
// Modul koji upravlja svim dolaznim HTTP zahtjevima, servira web sučelje 
// (HTML/JS/CSS) i pruža REST API za kontrolu antene i očitanje senzora.
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// POMOĆNE FUNKCIJE ZA WEB SERVER
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Obrada zahtjeva za početnu stranicu (Root - "/")
// ---------------------------------------------------------------------------
void handleRoot() {
    if (LittleFS.exists("/index.html")) {
        File file = LittleFS.open("/index.html", "r");
        server.streamFile(file, getContentType("/index.html"));
        file.close();
    } else {
        server.send(500, "text/plain", "Greska: index.html nije pronadjen!");
    }
}

// ---------------------------------------------------------------------------
// Obrada nepostojećih ruta (404 Not Found)
// ---------------------------------------------------------------------------
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

// ---------------------------------------------------------------------------
// API ZA GRAFIKON I SENZORE
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Vraća trenutne podatke senzora za grafikon (JSON)
// ---------------------------------------------------------------------------
void handleDataGraf() {
    float temperature = getTemperature();
    float humidity = getHumidity();

    JsonDocument doc; 
    if (temperature == -999.0 || humidity == -999.0) {
        doc["temp"] = 0.0;
        doc["hum"] = 0.0;
        doc["error"] = "Senzor nedostupan";
    } else {
        doc["temp"] = temperature; 
        doc["hum"] = humidity;     
    }

    String res;
    serializeJson(doc, res);
    server.send(200, "application/json", res);
}



void handleGetHistory() {
    server.send(200, "text/plain", getLogContent("/data_log.csv"));
}

// ---------------------------------------------------------------------------
// Briše CSV datoteku s povijesnim podacima mjerenja
// ---------------------------------------------------------------------------
void handleClearHistory() {
    clearLog("/data_log.csv");
    server.send(200, "text/plain", "OK");
}



// ---------------------------------------------------------------------------
// API ruta koja vraća trenutna očitanja svih senzora
// ---------------------------------------------------------------------------
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

// ---------------------------------------------------------------------------
// API ruta za dohvaćanje trenutnog UTC formata
// ---------------------------------------------------------------------------
void handleUtcTime() {
    JsonDocument doc;
    doc["time"] = getFormattedUTCTime();
    String res;
    serializeJson(doc, res);
    server.send(200, "application/json", res);
}



// ---------------------------------------------------------------------------
// Vraća informacije o statusu/tipu trenutno odabrane antene
// ---------------------------------------------------------------------------
void handleAntennaStatus() {
    JsonDocument doc;
    doc["status"] = getAntennaType();
    String res;
    serializeJson(doc, res);
    server.send(200, "application/json", res);
}

// ---------------------------------------------------------------------------
// Postavljanje vrste antene putem POST zahtjeva
// ---------------------------------------------------------------------------
void handleSetAntenna() {
    if (server.hasArg("plain")) {
        JsonDocument doc;
        deserializeJson(doc, server.arg("plain"));
        String type = doc["type"].as<String>();
        setAntennaType(type);
        server.send(200, "text/plain", "OK");
    }
}

// ---------------------------------------------------------------------------
// Vraća trenutni kut servo motora
// ---------------------------------------------------------------------------
void handleServoAngle() {
    JsonDocument doc;
    doc["angle"] = getCurrentServoAngle();
    String res;
    serializeJson(doc, res);
    server.send(200, "application/json", res);
}

// ---------------------------------------------------------------------------
// Postavlja novi kut servo motora
// ---------------------------------------------------------------------------
void handleSetAngle() {
    if (server.hasArg("plain")) {
        JsonDocument doc;
        deserializeJson(doc, server.arg("plain"));
        int receivedAngle = doc["angle"].as<int>();
        setServoAngle(receivedAngle);
        server.send(200, "text/plain", "OK");
    }
}

// ---------------------------------------------------------------------------
// Kalibracija servo motora
// ---------------------------------------------------------------------------
void handleCalibrateServo() {
    calibrateServo();
    server.send(200, "text/plain", "OK");
}



// ---------------------------------------------------------------------------
// Dohvaća sve spremljene logove u JSON formatu
// ---------------------------------------------------------------------------
void handleGetLogs() {
    server.send(200, "application/json", getAllLogsAsJson());
}

// ---------------------------------------------------------------------------
// API za spremanje novog unosa (veze) u evidenciju
// ---------------------------------------------------------------------------
void handleSaveLog() {
    if (server.hasArg("plain")) {
        JsonDocument doc;
        deserializeJson(doc, server.arg("plain"));
        LogEntry entry;
        entry.time = doc["time"].as<String>();
        entry.callsign = doc["callsign"].as<String>();
        entry.country = doc["country"].as<String>(); 
        entry.name = doc["name"].as<String>();
        entry.frequency = doc["frequency"].as<float>();
        entry.modulation = doc["modulation"].as<String>();
        entry.rstSent = doc["rstSent"].as<String>();
        entry.rstReceived = doc["rstReceived"].as<String>();
        entry.qthlocator = doc["qthlocator"].as<String>();
        entry.sentNr = doc["sentNr"].as<String>();
        entry.rcvdNr = doc["rcvdNr"].as<String>();
        entry.notes = doc["notes"].as<String>();
        addLog(entry);
        server.send(200, "text/plain", "OK");
    }
}

// ---------------------------------------------------------------------------
// Pojedinačno brisanje loga pomoću indeksa
// ---------------------------------------------------------------------------
void handleDeleteLog() {
    int index = server.arg("index").toInt();
    if (deleteLog(index)) server.send(200, "text/plain", "OK");
    else server.send(400, "text/plain", "Greska");
}

// ---------------------------------------------------------------------------
// Brisanje apsolutno svih logova
// ---------------------------------------------------------------------------
void handleClearAllLogs() {
    clearAllLogs();
    server.send(200, "text/plain", "OK");
}



// ---------------------------------------------------------------------------
// Inicijalizacija web servera i ruta
// ---------------------------------------------------------------------------
void initWebServer() {
    server.on("/", HTTP_GET, handleRoot);
    
    server.on("/logger", HTTP_GET, []() {
        if (LittleFS.exists("/logger.html")) {
            File file = LittleFS.open("/logger.html", "r");
            server.streamFile(file, "text/html");
            file.close();
        }
    });

    server.on("/data", HTTP_GET, handleDataGraf);
    server.on("/api/history", HTTP_GET, handleGetHistory);           
    server.on("/api/clear_history", HTTP_GET, handleClearHistory);   

    server.on("/api/sensor_readings", HTTP_GET, handleSensorReadings);
    server.on("/api/antenna_status", HTTP_GET, handleAntennaStatus);
    server.on("/api/set_antenna", HTTP_POST, handleSetAntenna);
    server.on("/api/servo_angle", HTTP_GET, handleServoAngle);
    server.on("/api/set_angle", HTTP_POST, handleSetAngle);
    server.on("/api/calibrate_servo", HTTP_POST, handleCalibrateServo);
    server.on("/api/utc_time", HTTP_GET, handleUtcTime);

    server.on("/logs", HTTP_GET, handleGetLogs);
    server.on("/save_log", HTTP_POST, handleSaveLog);
    server.on("/delete_log", HTTP_GET, handleDeleteLog);
    server.on("/clear_all_logs", HTTP_GET, handleClearAllLogs);

    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("Web server spreman! Podrška za povijest grafikona omogućena.");
}

// ---------------------------------------------------------------------------
// Posluživanje klijentskih zahtjeva (handleClient)
// ---------------------------------------------------------------------------
void handleClientRequests() {
    server.handleClient();
}