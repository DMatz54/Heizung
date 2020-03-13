// ****************************************************************
// Sketch Esp8266 Betriebsstundenzähler Dual Modular(Tab)
// created: Jens Fleischer, 2018-11-25
// last mod: Jens Fleischer, 2019-02-26
// For more information visit: https://fipsok.de
// ****************************************************************
// Hardware: Esp8266
// Software: Esp8266 Arduino Core 2.4.2 / 2.5.2 / 2.6.3
// Getestet auf: Nodemcu, Wemos D1 Mini Pro, Sonoff Dual
/******************************************************************
  Copyright (c) 2018 Jens Fleischer. All rights reserved.

  This file is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This file is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
*******************************************************************/
// Diese Version von Betriebsstundenzähler sollte als Tab eingebunden werden.
// #include <FS.h> muss im Haupttab aufgerufen werden
// Die Funktion "setupHobbsMeter();" muss im Setup aufgerufen werden.
// Der Admin Tab ist zum ausführen erforderlich.
/**************************************************************************************/

uint32_t totalmin[2], lastmin[2];

void setupHobbsMeter() {                                     // Betriebstunden(minuten) beim Neustart einlesen
  File file = SPIFFS.open("/betrieb.txt", "r");
  if (file) {
    lastmin[0] = totalmin[0] = file.parseInt();
    lastmin[1] = totalmin[1] = file.parseInt();
    file.close();
  }
}

void hobbsMeter(bool &state_0, bool &state_1) {              // Aufrufen mit Relais Status
  static uint32_t letzteMillis, previousMillis;
  auto aktuelleMillis = millis();
  if (aktuelleMillis - letzteMillis >= 6e4) {
    letzteMillis = aktuelleMillis;
    if (state_0 == aktiv) totalmin[0]++;                     // Betriebstundenzähler Relais 1 wird um eine Minute erhöht
    if (state_1 == aktiv) totalmin[1]++;                     // Betriebstundenzähler Relais 2 wird um eine Minute erhöht
  }
  if (aktuelleMillis - previousMillis >= 1728e5 && (totalmin[0] != lastmin[0] || totalmin[1] != lastmin[1])) {       // aller 2 Tage Betriebsstunden in Datei schreiben
    previousMillis = aktuelleMillis;
    lastmin[0] = totalmin[0];
    lastmin[1] = totalmin[1];
    speichern();
  }
}

String extra() {                                            // Betriebsstundenanzeige admin.html
  return "\", \"HourMeter1\":\"" + operatingTime(totalmin[0]) + "\", \"HourMeter2\":\"" +  operatingTime(totalmin[1]);
}

String operatingTime(uint32_t &tmin) {                       // lesbare Anzeige der Betriebstunden(minuten)
  char buf[9];
  snprintf(buf, sizeof(buf), "%d,%d", tmin / 60, tmin / 6 % 10);
  return buf;
}

void speichern() {
  File file = SPIFFS.open("/betrieb.txt", "w");              // Betriebstunden(minuten) speichern
  if (file && freeSpace(100)) {
    file.printf("%d\n%d\n", totalmin[0], totalmin[1]);
    file.close();
  }
}
