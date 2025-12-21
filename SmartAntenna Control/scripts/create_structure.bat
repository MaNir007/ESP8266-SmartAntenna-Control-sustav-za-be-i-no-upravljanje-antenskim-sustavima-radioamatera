@echo off
set "ROOT=SmartAntenna_Control\src"

REM --- Popis modula sustava ---
set DIRS=config wifi web sensors antenna logger time filesystem common

echo Kreiranje razvojne strukture projekta u: %ROOT%

REM Kreiranje osnovnog direktorija i glavne datoteke programa
if not exist "%ROOT%" mkdir "%ROOT%"
echo // Glavna datoteka sustava > %ROOT%\main.cpp

REM Kreiranje poddirektorija za modularni pristup
for %%D in (%DIRS%) do (
    if not exist "%ROOT%\%%D" mkdir "%ROOT%\%%D"
)

REM --- Inicijalizacija zaglavlja (.h) i izvornih datoteka (.cpp) ---

echo // Konfiguracija mreze i pinova > %ROOT%\config\config.h

echo // Upravljanje Wi-Fi vezom > %ROOT%\wifi\wifi_manager.h
echo #include "wifi_manager.h" > %ROOT%\wifi\wifi_manager.cpp

echo // HTTP Server i API rute > %ROOT%\web\web_server.h
echo #include "web_server.h" > %ROOT%\web\web_server.cpp

echo // Akvizicija podataka sa senzora > %ROOT%\sensors\sensor_manager.h
echo #include "sensor_manager.h" > %ROOT%\sensors\sensor_manager.cpp

echo // Kontrola step motora i releja > %ROOT%\antenna\antenna_control.h
echo #include "antenna_control.h" > %ROOT%\antenna\antenna_cpp

echo // Upravljanje QSO dnevnikom > %ROOT%\logger\log_manager.h
echo #include "log_manager.h" > %ROOT%\logger\log_manager.cpp

echo // Sinkronizacija vremena (NTP) > %ROOT%\time\time_sync.h
echo #include "time_sync.h" > %ROOT%\time\time_sync.cpp

echo // Rad s LittleFS datotecnim sustavom > %ROOT%\filesystem\fs_utils.h
echo #include "fs_utils.h" > %ROOT%\filesystem\fs_utils.cpp

echo // Globalni tipovi podataka > %ROOT%\common\common_types.h
echo // Pomocne funkcije sustava > %ROOT%\common\common_functions.h

echo.
echo [INFO] Projektna struktura za 'SmartAntenna Control' je uspjesno stvorena.
pause