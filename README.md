# 📡 ESP8266 SmartAntenna Control & Logging System

### Professional Radio Amateur Antenna Control, Environmental Monitoring and QSO Logging

![Version](https://img.shields.io/badge/version-2.1.0-blue.svg) ![Platform](https://img.shields.io/badge/platform-ESP8266-green.svg) ![Language](https://img.shields.io/badge/language-C%2B%2B%20%2F%20JavaScript-orange.svg)

**SmartAntenna Control** je sveobuhvatan, profesionalni sustav za upravljanje radioamaterskim antenskim sustavima. Uz preciznu kontrolu rotacije antene i prebacivanje između više antena putem web sučelja, sustav integrira napredni radioamaterski dnevnik (QSO Log) s podrškom za izvoz u natjecateljske formate te sustav za praćenje okolišnih uvjeta u realnom vremenu.

---

## 🚀 Glavne Značajke (Key Features)

### 1. Upravljanje Antenama (Antenna Control)

- **Rotacija (Stepper Motor):** Precizno upravljanje usmjerenim antenama (npr. Yagi) pomoću koračnog motora (2048 koraka/okret). Podržana kalibracija nulte točke.
- **Prebacivanje (Relay Switch):** Trenutno prebacivanje između različitih tipova antena (npr. GP dipol vs Yagi) putem bešumnih releja.
- **Web UI Control:** Responzivno sučelje s vizualnim prikazom trenutnog statusa.

### 2. Profesionalni Radio Dnevnik (Advanced QSO Logger)

- **Real-time Prefix Lookup:** Automatsko prepoznavanje države (DXCC), prikaz zastave i statusa (NEW ONE / Worked) na temelju prefiksa pozivne oznake.
- **QRZ.com Integracija:** Brzi pristup profilu sugovornika jednim klikom.
- **Maidenhead Locator:** Automatski izračun QTH lokatora (npr. JN75xt) pomoću GPS koordinata.
- **Višestruki Izvoz (Export):**
  - **ADIF (.adif):** Standardni format za razmjenu logova (eQSL, LoTW).
  - **EDI (REG1TEST):** Profesonalni format za VHF/UHF natjecanja (9A Robot/UCXLog).
  - **CSV (.csv):** Format za analizu podataka u proračunskim tablicama.

### 3. Monitoring i Senzori (Environment Monitoring)

- **Klimatski uvjeti:** DHT22 senzor za temperaturu i relativnu vlažnost zraka.
- **GPS Pozicioniranje:** Integrirani GPS modul za sinkronizaciju vremena i točne koordinate.
- **Napon sustava:** Praćenje napona napajanja putem ADC ulaza (A0).
- **Dijagrami:** Vizualni prikaz povijesti kretanja temperature i vlage u zadnjih 24h.

---

## 🛠 Hardverska Specifikacija (Hardware)

| Komponenta          | Model / Opis                  | Pin (ESP8266)    |
| :------------------ | :---------------------------- | :--------------- |
| **MCU**             | NodeMCU ESP8266 (v3)          | -                |
| **Stepper Motor**   | 28BYJ-48 s ULN2003 driverom   | D6, D7, D5, D0   |
| **Relay Module**    | 5V / 12V 1-kanalni (ili više) | D2               |
| **Temp/Hum Senzor** | DHT22 (AM2302)                | D1               |
| **GPS Modul**       | NEO-6M / NEO-7M               | D4 (RX), D3 (TX) |
| **Napajanje**       | 5V DC (min 1.5A)              | VIN / 5V         |

---

## 📁 Struktura Projekta (Software Architecture)

- **`src/`**: C++ izvorni kod (Arduino/ESP8266 platforma)
  - `antenna/`: Upravljanje stepper motorom i relejima.
  - `sensors/`: Integracija GPS-a i DHT22 senzora.
  - `web/`: REST API poslužitelj i rukovanje zahtjevima.
  - `logger/`: Upravljanje QSO bazom podataka i ANSI serijskom konzolom.
  - `filesystem/`: Rad s LittleFS (JSON spremanje).
- **`data/`**: Web UI resursi (prenose se na ESP8266 putem LittleFS)
  - `index.html`: Glavna kontrolna ploča (Dashboard).
  - `logger.html`: Sučelje radio dnevnika.
  - `data_graf.html`: Interaktivni grafikoni senzora.
  - `edi_export.html`: Napredni obrazac za kontest izvoz.
  - `common.js`: Zajedničke mrežne i pomoćne funkcije.

---

## 💻 Instalacija i Konfiguracija

### 1. Arduino IDE Postvke

1. Instalirajte **ESP8266 Core** (v3.0.0+).
2. Instalirajte potrebne biblioteke:
   - `ArduinoJson` (v7.x)
   - `AccelStepper`
   - `DHT sensor library`
   - `TinyGPS++`
   - `NTPClient`

### 2. Konfiguracija WiFi mreže

Otvorite `src/config/config.h` i postavite svoje podatke:

```cpp
constexpr char WIFI_SSID[]     = "VAS_WIFI_NAZIV";
constexpr char WIFI_PASSWORD[] = "VASA_LOZINKA";
```

### 3. Prijenos Web Datoteka

Koristite **ESP8266 LittleFS Data Upload** alat kako biste prenijeli sadržaj `data/` foldera na memoriju mikrokontrolera.

---

## 📡 Upotreba (Usage Guide)

1. **Dashboard:** Pratite temperaturu, vlagu i napon. Odaberite antenu i kut pod kojim želite zračiti (0-360°).
2. **Logging:** Tijekom rada, unesite pozivnu oznaku sugovornika. Sustav će automatski odrediti državu i trenutno vrijeme. Možete spremiti bilješke i RST izvještaje.
3. **Contest Export:** Na kraju natjecanja, kliknite na "Export to EDI", ispunite podatke o svojoj opremi i preuzmite datoteku spremnu za slanje robotu.
4. **Diagnostics:** Spojite se na Serijski monitor (115200 baud) za pregled detaljne dijagnostike u boji.

---

## 🔮 Planirane Nadogradnje (Roadmap)

- [ ] **WiFiManager:** Implementacija sučelja za konfiguraciju WiFi-a bez mjenjanja koda.
- [ ] **Multi-Relay Support:** Podrška za izbor između 4 ili 8 antena putem proširene konfiguracije.
- [ ] **MQTT Integracija:** Mogućnost slanja podataka na kućnu automatizaciju (npr. Home Assistant).
- [ ] **PWA Offline Mode:** Poboljšana podrška za rad bez pristupa ESP-u (keširanje logova lokalno u pregledniku).
- [ ] **Vatmetar / SWR Metar:** Dodavanje modula za mjerenje izlazne snage i SWR-a u realnom vremenu.

---

## 🤝 Doprinosi i Licenca

Projekt je otvoren za sve radioamatere. Ako želite doprinijeti, otvorite _Pull Request_ ili prijavite bug putem _Issues_.

**73 de 9A5AGN**
