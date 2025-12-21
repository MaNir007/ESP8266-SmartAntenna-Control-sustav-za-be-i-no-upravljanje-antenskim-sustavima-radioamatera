#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <Arduino.h>
#include <vector>
#include "../common/common_types.h" // OVO MORA BITI TU!

// NEMA DEFINICIJE struct LogEntry OVDJE!

// Javne funkcije
void initLogManager();
void addLog(const LogEntry& newLog);
String getAllLogsAsJson();
bool deleteLog(int index);
bool clearAllLogs();

// Deklaracija globalnog vektora logova (DEFINICIJA je u .cpp)
extern std::vector<LogEntry> radioLogs;

#endif