# Automatisierte Pflanzenbewässerungsanlage  
*(Industrie 4.0 – ESP32, Cloud & Blynk/AWS IoT)*

Dieses Repository enthält den Code und die Projektdokumentation einer **autonomen, cloud-fähigen Pflanzenbewässerungsanlage** auf Basis eines ESP32.  
Das Projekt wurde im Rahmen des Studiengangs Wirtschaftsingenieurwesen, Vertiefung Industrie 4.0, an der Fachhochschule Wedel umgesetzt.

---

## Projektüberblick

Ziel des Projekts ist eine **benutzerfreundliche, erweiterbare Bewässerungsanlage**, die:

- automatisch nach Bodenfeuchtigkeit bewässert,
- Temperatur, Luftfeuchtigkeit und Bodenfeuchte misst,
- einen Wassertank überwacht (Low-Level-Alarm),
- Messwerte in die Cloud (AWS IoT oder Blynk) überträgt,
- Werte lokal auf einem OLED-Display anzeigt,
- den Nutzer bei leerem Tank aktiv informiert (LED + Benachrichtigung).

Alle Komponenten sind in einer **Holzbox mit integriertem Tank** verbaut, sodass Elektronik und wasserführende Teile räumlich getrennt und geschützt sind.

---

## Features

- **Automatische Bewässerung**
  - Bewässerung auf Basis der Bodenfeuchtigkeit.
  - Bewässerung startet erst unterhalb eines definierten Feuchte-Schwellwerts (z. B. < 40 %).

- **Mehrere Sensoren**
  - Bodenfeuchtesensor im Topf (analog, kalibrierbar).
  - Bodenfeuchtesensor im Tank als **Füllstandsensor** (leer/voll).
  - DHT11 oder DHT22 für Temperatur & Luftfeuchtigkeit (AB-Vergleich beider Sensoren).

- **Cloud-Anbindung**
  - MQTT-Übertragung der Messwerte zu **AWS IoT** *oder*
  - Nutzung der **Blynk Cloud** mit Mobile-App, Widgets & Push-Benachrichtigungen.

- **Lokale Anzeige**
  - 0,96" I²C-OLED-Display (128×64) mit Anzeige von:
    - Bodenfeuchte,
    - Temperatur,
    - Luftfeuchtigkeit. :contentReference[oaicite:5]{index=5}
  - Status-LED für niedrigen Wasserstand im Tank.

- **Sicherheit & Zuverlässigkeit**
  - Tank-Überwachung: Pumpe stoppt bei leerem Tank.
  - Optokoppler-Relais zur galvanischen Trennung.
  - Pufferkondensator zur Vermeidung von Spannungseinbrüchen beim Pumpenstart.
  - Kalman-Filter zur Unterdrückung von Mess-Rauschen und Ausreißern der Bodenfeuchtesensoren. 

- **Erweiterbarkeit**
  - Architektur ist auf mehrere Pflanzen / Sensoren ausgelegt.
  - Unterschiedliche „Profile“ für verschiedene Pflanzenarten sind im Code vorgesehen.

---

## Hardware

Verwendete Kernkomponenten (Beispiele, genaue Teile können je nach Aufbau variieren):

- **Controller**
  - ESP32-Board (WiFi-fähig, 3,3 V-Logik).

- **Sensoren**
  - DHT11 *oder* DHT22 (Temperatur & Luftfeuchtigkeit).
  - Bodenfeuchtesensor, kalibrierbar (analog, für Topf).
  - Bodenfeuchtesensor V1.2 (Tank-Füllstand, einfaches trocken/feucht-Signal). 

- **Aktoren & Ansteuerung**
  - 3,3 V-Tauchpumpe im Tank.
  - 2-fach Optokoppler-Relaismodul (galvanisch getrennt).

- **Anzeige & Status**
  - 0,96" I²C-OLED-Display (128×64).
  - LED + Vorwiderstand zur Tankanzeige.

- **Stromversorgung**
  - 5 V-Netzteil (z. B. 50 W).
  - Pufferkondensator (z. B. 1000 µF).
  - Vorwiderstände (z. B. für LED, Pumpe, etc.).

- **Mechanik**
  - Holzbox mit Deckel und Kabeldurchführungen.
  - Externer Tank (z. B. PE-Behälter) im Inneren der Box.

---

## Architektur

### Datenfluss

1. ESP32 liest:
   - Bodenfeuchte (Topf) analog,
   - Tank-Sensor,
   - Temperatur & Luftfeuchtigkeit (DHT11/DHT22).

2. Rohdaten werden gefiltert (Kalman-Filter für Bodenfeuchte).

3. Logik:
   - Wenn **Bodenfeuchte < Feuchte-Schwellwert** *und* **Tank nicht leer**  
     → Relais schaltet Pumpe in kurzen Intervallen.
   - Wenn Tank-Sensor „trocken“ meldet  
     → LED an der Box an, Meldung in Cloud/Blynk.

4. Anzeige & Cloud:
   - Messwerte → OLED-Display über I²C.
   - Messwerte → MQTT (AWS IoT) oder Blynk-Cloud (per Bibliothek).

---

## Getting Started

> Die folgenden Schritte sind allgemein gehalten. Passe Pfade, Board-Typ, Pins & Credentials an dein konkretes Repo und Setup an.

### 1. Voraussetzungen

- Arduino IDE oder PlatformIO
- ESP32-Boardpaket in der Arduino IDE installiert
- Bibliotheken (mindestens):
  - `WiFi.h` / `WiFiClientSecure.h`
  - `PubSubClient` (für AWS IoT)
  - `ArduinoJson`
  - `Blynk`-Bibliothek (für Blynk-Variante)
  - `Adafruit_SSD1306` (OLED)
  - `Adafruit_GFX`
  - DHT-Bibliothek (`DHT.h`)
  - Kalman-Filter-Bibliothek (z. B. `Kalman Filter Library` für Arduino)

> **Hinweis:** Die Bibliotheks-Namen können je nach deiner Implementierung im Code leicht variieren.

### 2. Repository klonen

```bash
git clone [https://github.com/<user>/<repo>.git](https://github.com/MHRN-DS/AutomatedPlant)
cd <[repo](https://github.com/MHRN-DS/AutomatedPlant)>
