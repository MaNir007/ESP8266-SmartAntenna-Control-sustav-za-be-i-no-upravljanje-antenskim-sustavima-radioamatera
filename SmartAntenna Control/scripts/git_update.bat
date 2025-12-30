@echo off
:: Ulazi u mapu gdje je skripta
cd /d "%~dp0"

:: Skok u root direktorij projekta (tamo gdje je .git mapa)
for /f "delims=" %%i in ('git rev-parse --show-toplevel') do cd /d "%%i"

echo --- Zapocinjem slanje promjena na GitHub ---
echo Lokacija projekta: %cd%
echo.

:: Prvo dodajemo SVE promjene iz svih mapa
git add -A

:: Sada prikazujemo status da vidis sto ce se poslati
echo --- Datoteke spremne za slanje: ---
git status --short
echo.

:: Unos poruke
set /p msg="Update: "

:: Commit i Push
git commit -m "%msg%"
git push origin main

echo.
echo --- Gotovo! ---
pause