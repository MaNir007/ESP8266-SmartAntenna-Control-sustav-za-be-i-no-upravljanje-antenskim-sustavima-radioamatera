@echo off
cd /d "%~dp0"
for /f "delims=" %%i in ('git rev-parse --show-toplevel') do cd /d "%%i"

echo --- Zapocinjem uskladjivanje s GitHubom ---
echo Lokacija: %cd%
echo.

:: 1. Ponisti trenutni "stage" (rjesava index error)
:: Ovo ne brise tvoj kod, samo dopusta Pull
git reset

:: 2. Dohvati promjene s GitHuba
echo Dohvacam nove promjene s servera...
git pull origin main --rebase

echo.
:: 3. Dodaj SVE lokalne promjene (ukljucujuci index.html i .docx)
git add -A

echo --- Datoteke spremne za slanje: ---
git status --short
echo.

:: 4. Unos poruke (provjera da poruka nije prazna)
:input
set "msg="
set /p msg="Update (upisi opis promjene): "
if "%msg%"=="" (
    echo Greska: Moras upisati poruku commita!
    goto input
)

:: 5. Commit i Push
git commit -m "%msg%"
git push origin main

echo.
echo --- Gotovo! Sve je na GitHubu. ---
pause