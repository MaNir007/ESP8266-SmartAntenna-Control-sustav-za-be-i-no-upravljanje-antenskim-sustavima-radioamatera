// ---------------------------------------------------------------------------
// log_manager.h - Sučelje za upravljanje evidencijom radio veza
// ---------------------------------------------------------------------------
// Definira tipove i funkcije za trajno spremanje i dohvat logova.
// Koristi JSON format i LittleFS za pohranu podataka.
// ---------------------------------------------------------------------------
#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <Arduino.h>
#include <vector>
#include "../common/common_types.h"

// Javne funkcije
void initLogManager();
void addLog(const LogEntry& newLog);
String getAllLogsAsJson();
bool deleteLog(int index);
bool clearAllLogs();

extern std::vector<LogEntry> radioLogs;

#endif