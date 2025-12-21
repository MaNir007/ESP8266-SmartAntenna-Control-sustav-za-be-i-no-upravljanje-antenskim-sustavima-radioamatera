# 📡 ESP8266 SmartAntenna Control
### Sustav za bežično upravljanje antenskim sustavima radioamatera (Remote Antenna Switch)

**ESP8266 SmartAntenna Control** je modularni sustav koji omogućuje daljinsko upravljanje antenskim preklopnicima, rotatorima i drugim elementima antenskog sustava pomoću ESP8266 mikrokontrolera.  
Projekt je namijenjen radioamaterima koji žele **pouzdano, sigurno i jednostavno upravljanje antenama** putem web sučelja, dostupnog s računala, mobitela ili tableta unutar lokalne mreže.  
Sustav je pogodan kako za **fiksne postaje**, tako i za **terenski rad (POTA / SOTA / portable)**.

---

## 🚀 Glavne značajke

- 🌐 **Web sučelje**  
  Intuitivno, responzivno web sučelje za upravljanje relejima jednim klikom.

- 📶 **Dva načina rada**
  - **Station Mode** – spajanje na postojeću kućnu Wi-Fi mrežu  
  - **Access Point (AP) Mode** – ESP8266 stvara vlastitu Wi-Fi mrežu (idealno za teren)

- 🔄 **Status u realnom vremenu**  
  Trenutno aktivna antena prikazana je istovremeno na svim povezanim uređajima.

- 🔒 **Interlock zaštita**  
  Softverska zaštita koja sprječava istovremeno uključivanje više antena.

- 🔄 **OTA (Over-The-Air) ažuriranje**  
  Bežično ažuriranje firmvera bez USB kabela.

---

## 🛠 Hardverska specifikacija

| Komponenta | Preporučeni model | Napomena |
|-----------|------------------|---------|
| MCU | ESP8266 (NodeMCU / D1 Mini) | 80 MHz / 160 MHz |
| Releji | 4- ili 8-kanalni relejni modul | Opto-izolirani (preporučeno) |
| Napajanje | 5 V DC (min. 1 A) | Ovisi o broju releja |
| RF zaštita | Metalno kućište + feriti | Ključno za rad blizu predajnika |

---

## 🔌 Shema spajanja (primjer)

GND → GND relejnog modula
D1 → GPIO5 → Relej 1 (Antena 1)
D2 → GPIO4 → Relej 2 (Antena 2)
D5 → GPIO14 → Relej 3 (Antena 3)
D6 → GPIO12 → Relej 4 (Antena 4)

> ⚠️ Preporučuje se korištenje kratkih vodiča, feritnih prstenova i odvojenog napajanja.

---

## 💻 Instalacija i programiranje

### 1️⃣ Priprema razvojnog okruženja

U Arduino IDE (File → Preferences) dodajte sljedeći URL u *Additional Boards Manager URLs*:
http://arduino.esp8266.com/stable/package_esp8266com_index.json


Zatim instalirajte **ESP8266 platformu** putem *Boards Managera*.

---

### 2️⃣ Potrebne biblioteke

- `ESP8266WiFi`  
- `ESP8266WebServer`  
- `ArduinoOTA` *(opcionalno, za OTA)*

---

### 3️⃣ Konfiguracija

-U datoteci `config.h` (ili na vrhu glavnog koda) prilagodite mrežne postavke:
-const char* ssid = "VAS_WIFI_NAZIV";
-const char* password = "VASA_LOZINKA";

###📡 Upotreba (Operation)

##Pokretanje
-Spojite sustav na napajanje.

##Pristup web sučelju
-U pregledniku otvorite IP adresu uređaja (npr. http://192.168.1.100).

##Upravljanje antenama
-Klikom na naziv antene (npr. Dipole 80 m, Yagi 20 m) sustav:
-aktivira odabrani relej
-automatski isključuje prethodnu antenu (ako je interlock uključen)

###⚠️ Sigurnosne napomene (RF Safety)

##📡 RF povratna sprega
-Koristite blok kondenzatore (100 nF) i feritne jezgre na signalnim i naponskim vodovima, posebno pri većim izlaznim snagama.

##⚡ Radni napon releja
-Ako upravljate koaksijalnim relejima na 12V ili 24V, provjerite kompatibilnost i izolaciju.

###🖼 Slike
-Naziv	Opis	Putanja
-Web Interface	Responzivno web sučelje za upravljanje antenama	docs/images/web_interface.png
-Antenna Setup	Prikaz spajanja releja i antena	docs/images/antenna_setup.png
-System Block Diagram	Blok shema ESP8266 SmartAntenna Control sustava	docs/images/block_diagram.png
###🤝 Doprinosi i kontakt

-Slobodno otvorite Issue ili Pull Request ako želite doprinijeti projektu.

###👤 Autor

-ESP8266 SmartAntenna Control
-Radioamaterski projekt

##73 de 9A5AGN

