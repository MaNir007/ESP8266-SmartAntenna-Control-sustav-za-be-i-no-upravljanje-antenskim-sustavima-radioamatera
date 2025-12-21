ESP8266 SmartAntenna Control
Sustav za bežično upravljanje antenskim sustavima radioamatera.

Ovaj projekt omogućuje daljinsko upravljanje antenskim preklopnicima, rotatorima ili drugim elementima antenskog sustava koristeći ESP8266 mikrokontroler. Sustav je dizajniran kako bi radioamaterima olakšao rad putem intuitivnog web sučelja dostupnog s bilo kojeg uređaja unutar lokalne mreže.

🚀 Glavne značajke
Web sučelje: Upravljanje putem preglednika (mobitel, tablet, PC).

Bežična povezivost: Radi preko postojeće Wi-Fi mreže ili kao samostalna pristupna točka (Access Point).

Modularnost: Mogućnost proširenja za upravljanje više različitih antena ili releja.

Niska latencija: Brza odzivnost sustava ključna za natjecanja (contesting).

OTA (Over-The-Air) ažuriranje: Mogućnost nadogradnje softvera bez fizičkog spajanja na računalo.

🛠 Hardverski zahtjevi
Za sastavljanje sustava potrebne su sljedeće komponente:

Mikrokontroler: ESP8266 (npr. NodeMCU ili Wemos D1 Mini).

Relejni modul: 4-kanalni ili 8-kanalni relejni modul (ovisno o broju antena).

Napajanje: 5V DC (za ESP8266 i releje).

Kućište: Zaštitno kućište prilagođeno radu u RF okruženju.

💻 Instalacija i postavljanje
Klonirajte repozitorij:

Bash

git clone https://github.com/MaNir007/ESP8266-SmartAntenna-Control-sustav-za-be-i-no-upravljanje-antenskim-sustavima-radioamatera.git
Arduino IDE postavke:

Instalirajte Arduino IDE.

Dodajte ESP8266 podršku putem Board Managera.

Instalirajte potrebne biblioteke (npr. ESP8266WiFi, ESP8266WebServer).

Konfiguracija:

Otvorite .ino datoteku.

Unesite svoje Wi-Fi podatke (SSID i Password) u kodu.

Upload:

Povežite ESP8266 na računalo i učitajte kod.

📡 Kako koristiti
Nakon uspješnog učitavanja koda, otvorite Serial Monitor u Arduino IDE-u kako biste saznali IP adresu uređaja.

Upišite IP adresu u svoj web preglednik (npr. http://192.168.1.50).

Upravljajte antenama putem gumba na ekranu.

⚠️ Sigurnosna napomena
Pri radu s antenskim sustavima i visokim snagama (RF), osigurajte da su releji adekvatno izolirani i da sustav ima zajedničko uzemljenje kako bi se izbjegle smetnje ili oštećenja elektronike.

🤝 Doprinosi
Doprinosi projektu su dobrodošli! Ako imate ideje za poboljšanje sučelja ili nove funkcionalnosti, slobodno otvorite Issue ili pošaljite Pull Request.

73 de [Tvoj Pozivni Znak ili Ime]
