/**
 * @file web_server.cpp
 * @brief Implementacija web server modula.
 * @details Ovaj modul je odgovoran za pokretanje ESP8266 web servera,
 * definiranje ruta za statičke datoteke (HTML, CSS, JS) te rukovanje 
 * API zahtjevima za senzore, antenu, vrijeme i evidenciju radio veza (logove).
 */

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

/**
 * @brief Obrada zahtjeva za početnu stranicu (Root - "/").
 * Prikazuje index.html ili vraća grešku ako datoteka ne postoji.
 */
void handleRoot() {
    if (LittleFS.exists("/index.html")) {
        File file = LittleFS.open("/index.html", "r");
        server.streamFile(file, getContentType("/index.html"));
        file.close();
    } else {
        server.send(500, "text/plain", "Greska: index.html nije pronadjen!");
    }
}

/**
 * @brief Obrada nepostojećih ruta (404 Not Found).
 * Pokušava pronaći traženu datoteku u LittleFS-u, ako ne uspije vraća 404 grešku.
 */
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

// --- API ZA GRAFIKON (Novo i usklađeno s data_graf.js) ---

/**
 * @brief Vraća trenutne podatke senzora (temperatura i vlaga) za gafikon u obliku JSON-a.
 */
void handleDataGraf() {
    float temperature = getTemperature();
    float humidity = getHumidity();

    JsonDocument doc; 
    // Ako senzor ne radi, šaljemo 0 da graf ne pukne
    if (temperature == -999.0 || humidity == -999.0) {
        doc["temp"] = 0.0;
        doc["hum"] = 0.0;
        doc["error"] = "Senzor nedostupan";
    } else {
        doc["temp"] = temperature; // Ključ mora biti "temp" za JS
        doc["hum"] = humidity;     // Ključ mora biti "hum" za JS
    }

    String res;
    serializeJson(doc, res);
    server.send(200, "application/json", res);
}

// --- NOVO: Rukovanje povijesnim podacima za grafikon ---

/**
 * @brief Vraća povijesne podatke (CSV) za iscrtavanje grafova.
 */
void handleGetHistory() {
    // Dohvaćamo sadržaj CSV datoteke koju main.cpp puni u pozadini
    String history = getLogContent("/data_log.csv");
    
    // Šaljemo kao običan tekst (CSV), klijent će to sam parsirati
    server.send(200, "text/plain", history);
}

/**
 * @brief Briše CSV datoteku s povijesnim podacima mjerenja.
 */
void handleClearHistory() {
    clearLog("/data_log.csv");
    server.send(200, "text/plain", "OK");
}

// --- API ZA SENZORE I VRIJEME ---

/**
 * @brief API ruta koja vraća trenutna očitanja svih senzora (temperatura, vlaga, napon).
 */
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

/**
 * @brief API ruta za dohvaćanje trenutnog UTC formata s uređaja.
 */
void handleUtcTime() {
    JsonDocument doc;
    doc["time"] = getFormattedUTCTime();
    String res;
    serializeJson(doc, res);
    server.send(200, "application/json", res);
}

// --- API ZA ANTENU I STEPER ---

/**
 * @brief Vraća informacije o statusu/tipu trenutno odabrane antene.
 */
void handleAntennaStatus() {
    JsonDocument doc;
    doc["status"] = getAntennaType();
    String res;
    serializeJson(doc, res);
    server.send(200, "application/json", res);
}

/**
 * @brief Omogućava postavljanje vrste antene putem POST zahtjeva.
 */
void handleSetAntenna() {
    if (server.hasArg("plain")) {
        JsonDocument doc;
        deserializeJson(doc, server.arg("plain"));
        String type = doc["type"].as<String>();
        setAntennaType(type);
        server.send(200, "text/plain", "OK");
    }
}

/**
 * @brief Pokazuje trenutni kut servo motora.
 */
void handleServoAngle() {
    JsonDocument doc;
    doc["angle"] = getCurrentServoAngle();
    String res;
    serializeJson(doc, res);
    server.send(200, "application/json", res);
}

/**
 * @brief Postavlja novi kut servo motora.
 */
void handleSetAngle() {
    if (server.hasArg("plain")) {
        JsonDocument doc;
        deserializeJson(doc, server.arg("plain"));
        int receivedAngle = doc["angle"].as<int>();
        setServoAngle(receivedAngle);
        server.send(200, "text/plain", "OK");
    }
}

/**
 * @brief Ruta za pozivanje kalibracije servo motora.
 */
void handleCalibrateServo() {
    calibrateServo();
    server.send(200, "text/plain", "OK");
}

// --- LOG MANAGER API ---

/**
 * @brief Dohvaća sve spremljene logove u JSON formatu.
 */
void handleGetLogs() {
    server.send(200, "application/json", getAllLogsAsJson());
}

/**
 * @brief API za spremanje novog unosa (veze) u evidenciju.
 */
void handleSaveLog() {
    if (server.hasArg("plain")) {
        JsonDocument doc;
        deserializeJson(doc, server.arg("plain"));
        LogEntry entry;
        entry.time = doc["time"].as<String>();
        entry.callsign = doc["callsign"].as<String>();
        entry.country = doc["country"].as<String>(); // FIX: Dodan unos za državu!
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

/**
 * @brief Api za pojedinačno brisanje loga pomoću indeksa.
 */
void handleDeleteLog() {
    int index = server.arg("index").toInt();
    if (deleteLog(index)) server.send(200, "text/plain", "OK");
    else server.send(400, "text/plain", "Greska");
}

/**
 * @brief Ruta koja briše apsolutno sve logove.
 */
void handleClearAllLogs() {
    clearAllLogs();
    server.send(200, "text/plain", "OK");
}

// --- INICIJALIZACIJA ---

/**
 * @brief Inicijalizacija svih ruta web servera i definiranje HTTP metoda (GET, POST).
 */
void initWebServer() {
    // Osnovne stranice
    server.on("/", HTTP_GET, handleRoot);
    
    server.on("/logger", HTTP_GET, []() {
        if (LittleFS.exists("/logger.html")) {
            File file = LittleFS.open("/logger.html", "r");
            server.streamFile(file, "text/html");
            file.close();
        }
    });

    // Rute za grafikon
    server.on("/data", HTTP_GET, handleDataGraf);
    server.on("/api/history", HTTP_GET, handleGetHistory);           // NOVO: Dohvat CSV povijesti
    server.on("/api/clear_history", HTTP_GET, handleClearHistory);   // NOVO: Brisanje grafikona

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
    Serial.println("Web server spreman! Podrška za povijest grafikona omogućena.");
}

/**
 * @brief Petlja web servera za posluživanje klijentskih zahtjeva (mora se pozivati unutar loop-a).
 */
void handleClientRequests() {
    server.handleClient();
}