// ****************************************************************
// Sketch Esp8266 Zeitschaltuhr Dual Modular(Tab)
// created: Jens Fleischer, 2019-03-08
// last mod: Jens Fleischer, 2019-04-06
// For more information visit: https://fipsok.de
// ****************************************************************
// Hardware: Esp8266, Relais Modul o. Mosfet IRF3708 o. Fotek SSR-40 DA
// für Relais Modul
// GND an GND
// IN1 an D5 = GPIO14
// IN2 an D6 = GPIO12
// VCC an VIN -> je nach verwendeten Esp.. möglich
// Jumper JD-VCC VCC
// alternativ ext. 5V Netzteil verwenden
//
// für Mosfet IRF3708
// Source an GND
// Mosfet1 Gate an D5 = GPIO14
// Mosfet2 Gate an D6 = GPIO12
//
// für 3V Solid State Relais
// GND an GND
// SSR1 Input + an D5 = GPIO14
// SSR2 Input + an D6 = GPIO12
//
// Software: Esp8266 Arduino Core 2.4.2 / 2.5.2 / 2.6.3
// Getestet auf: Nodemcu, Wemos D1 Mini Pro, Sonoff Dual
/******************************************************************
  Copyright (c) 2019 Jens Fleischer. All rights reserved.

  This file is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This file is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
*******************************************************************/
// Diese Version von Wochenzeitschaltuhr sollte als Tab eingebunden werden.
// #include <FS.h> #include <ESP8266WebServer.h> müssen im Haupttab aufgerufen werden
// Die Funktionalität des ESP8266 Webservers ist erforderlich.
// Der Lokalzeit Tab ist zum ausführen der Zeitschaltuhr einzubinden.
// Die Funktion "setupSchaltUhr();" muss im Setup aufgerufen werden.
// Zum schalten muss die Funktion "dualSchaltuhr();" im loop(); aufgerufen werden.
/**************************************************************************************/

const auto aktiv = LOW;                       // LOW für LOW aktive Relais oder HIGH für HIGH aktive (zB. SSR, Mosfet) einstellen
const uint8_t relPin[] = {D5, D6};            // Pin für Relais einstellen
const auto schalten = 10;                     // Anzahl Schaltzeiten (analog Html Dokument) einstellen 2 bis 40
const auto kreise =5;                         // Anzahl der Heizkreise
char switchTime[schalten/2*kreise  * 2][6];
byte switchActive[schalten/2*kreise ];
byte wday[schalten/2*kreise ][7];
bool relState[] = {!aktiv, !aktiv};

void setupSchaltUhr() {
  for (auto& pin : relPin) digitalWrite(pin, !aktiv), pinMode(pin, OUTPUT);
  File file = SPIFFS.open("/szeit.dat", "r");
  if (file) {                                                          // Einlesen aller Daten falls die Datei im Spiffs vorhanden ist.
    file.read(switchActive, sizeof(switchActive));
    for (auto& elem : wday) {
      file.read(elem, sizeof(elem));
    }
    while (file.read() != '\n');
    for (auto& elem : switchTime) {
      file.readBytesUntil('\n', elem, sizeof(elem));
    }
    file.close();
  } else {                                                             // Sollte die Datei nicht existieren
    for (auto i = 0; i < schalten/2*kreise ; i++) {
      switchActive[i] = 1;                                             // werden alle Schaltzeiten
      for (auto j = 0; j < 6; j++) {
        wday[i][j] = 1 + j;                                            // und alle Wochentage aktiviert.
      }
    }
  }
  server.on("/timer", HTTP_POST, []() {
    if (server.args() == 1) {
      switchActive[server.argName(0).toInt()] = server.arg(0).toInt();
      printer();
      String temp = "\"";
      for (auto& elem : switchActive) {
        temp += elem;
      }
      temp += "\"";
      server.send(200, "application/json", temp);
    }
    if (server.hasArg("sTime")) {
      char str[schalten/2*kreise  * 14];
      strcpy (str, server.arg("sTime").c_str());           // Liest die vom Browser übergebene Werte nach Argument 
   Serial.println(str);                                    // 'sTime' in  die Variable 'str' ein
      char* ptr = strtok(str, ",");                        // zerlegt 'str' nach dem ',' 
      byte i = 0, j = 0;                 
      while (ptr != NULL) {
        strcpy (switchTime[i++], ptr);                     // kopiert array von 'str' nach 'swichTime'
        ptr = strtok(NULL, ",");
      }
      if (server.arg("sDay").length() == schalten/2*kreise  * 14 - 1) {
        strcpy (str, server.arg("sDay").c_str());
        ptr = strtok(str, ",");
        i = 0;
        while (ptr != NULL) {
          if (i > 6) {
            j++;
            i = 0;
          }
          wday[j][i++] = atoi(ptr);
          ptr = strtok(NULL, ",");
        }
        printer();
      }
      else {
        server.send(400, "");
      }
    }
    String temp = "[";
    for (auto& elem : switchTime) {
      if (temp != "[") temp += ',';
      temp += (String)"\"" + elem + "\"";
    }
    temp += ",\"";
    for (auto& elem : switchActive) {
      temp += elem;
    }
    for (auto& elem : wday) {
      temp += "\",\"";
      for (auto& wert : elem) {
        temp += wert;
      }
    }
    temp += "\"]";
    server.send(200, "application/json", temp);
  });
  server.on("/timer", HTTP_GET, []() {
    if (server.hasArg("tog") && server.arg(0) == "0") {
      relState[0] = !relState[0];                                 // Relais1 Status manuell ändern
      timeDataLogger(0, server.client().remoteIP().toString());   // Funktionsaufruf Zeitdatenlogger
    }
    if (server.hasArg("tog") && server.arg(0) == "1") {
      relState[1] = !relState[1];                                 // Relais2 Status manuell ändern
      timeDataLogger(1, server.client().remoteIP().toString());   // Funktionsaufruf Zeitdatenlogger
    }
    server.send(200, "application/json", (String)"[\"" + (relState[0] == aktiv) + "\",\"" + (relState[1] == aktiv) + "\",\"" + localTime() + "\"]");
  });
}

void printer() {
  File file = SPIFFS.open("/szeit.dat", "w");
  if (file) {
//Serial.printf("Switchactive = %s, Groesse= %d", switchActive, sizeof(switchActive));
    file.write(switchActive, sizeof(switchActive));
    for (auto& elem : wday) {
      file.write(elem, sizeof(elem));
    }
    for (auto& elem : switchTime) {                              // Schreibt 'switchTime in Datei
      file.printf("\n%s", elem);
    }
    file.close();
  }
}

void dualSchaltuhr() {
  static auto lastmin = -1;
  static uint8_t lastState[] = {aktiv, aktiv};
  hobbsMeter(relState[0], relState[1]);              // Funktionsaufruf Betriebsstundenzähler mit Relais Status
  if (tm.tm_min != lastmin) {
    lastmin = tm.tm_min;
    char buf[6];
    snprintf(buf, sizeof(buf), "%.2d:%.2d", tm.tm_hour, tm.tm_min);
    for (auto i = 0; i < schalten * 2; i++) {
      if (switchActive[i / 2] && !strcmp(switchTime[i], buf)) {
        for (auto j = 0; j < 7; j++) {
          if (tm.tm_wday == wday[i / 2][j]) {
            i < (schalten % 2 ? schalten + 1 : schalten) ? ({relState[0] = i % 2 ? !aktiv : aktiv; timeDataLogger(0, "Programm");}) : ({relState[1] = i % 2 ? !aktiv : aktiv; timeDataLogger(1, "Programm");});  // Relais Status nach Zeit ändern
          }
        }
      }
    }
  }
  if (relState[0] != lastState[0] || relState[1] != lastState[1]) {    // Relais schalten wenn sich der Status geändert hat
    for (auto i = 0; i < 2; i++) {
      lastState[i] = relState[i];
      digitalWrite(relPin[i], relState[i]);
      Serial.println((String)"Relais" + (1 + i) + (digitalRead(relPin[i]) == aktiv ? " an" : " aus"));
    }
  }
}
