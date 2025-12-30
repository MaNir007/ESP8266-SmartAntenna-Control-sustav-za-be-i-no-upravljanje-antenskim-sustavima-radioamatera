@echo off
cd /d "%~dp0"
for /f "delims=" %%i in ('git rev-parse --show-toplevel') do cd /d "%%i"

echo --- Zapocinjem uskladjivanje s GitHubom ---
echo Lokacija: %cd%
echo.

:: 1. Prvo povuci promjene s GitHuba (rjesava tvoj error)
echo Dohvacam nove promjene s servera...
git pull origin main --rebase

echo.
:: 2. Dodaj sve tvoje lokalne promjene
git add -A

echo --- Datoteke spremne za slanje: ---
git status --short
echo.

:: 3. Unos poruke
set /p msg="Update: "

:: 4. Commit i Push
git commit -m "%msg%"
git push origin main

echo.
echo --- Gotovo! Sve je sinkronizirano. ---
pause