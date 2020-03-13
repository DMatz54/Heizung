// ****************************************************************
// Sketch Esp8266 Zeitdatenlogger Dual Modular(Tab)
// created: Jens Fleischer, 2019-03-31
// last mod: Jens Fleischer, 2019-04-06
// For more information visit: https://fipsok.de
// ****************************************************************
// Hardware: Esp8266
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
// Diese Version von Ereignisdatenspeicher sollte als Tab eingebunden werden.
// #include <FS.h> muss im Haupttab aufgerufen werden
// Der Lokalzeit Tab ist zum ausführen des Zeitdatenlogger erforderlich.
// Der Spiffs Tab ist zum ausführen erforderlich.
/**************************************************************************************/

void timeDataLogger(const uint8_t num, const String customer) {     // Relais Nummer und Ereignisauslöser
  static bool lastrelState[] {!aktiv, !aktiv};
  if (relState[num] != lastrelState[num]) {                         // Prüft ob sich der Relais Status geändert hat.
    char fileName[23];
    snprintf(fileName, sizeof(fileName), "/Relais%d_%s.csv", num + 1, monthNames[tm.tm_mon]);
    if (!SPIFFS.exists(fileName)) {                                 // Logdatei für den aktuellen Monat anlegen falls nicht vorhanden.
      File f = SPIFFS.open(fileName, "a");
      if (f && freeSpace(40)) {                                     // Prüft ob genügend Speicher frei ist.
        f.printf("%s;An;Initiator;Aus;Initiator\n", monthShortNames[tm.tm_mon]); // Kopfzeile schreiben
      }
      f.close();
      char path[23];
      snprintf(path, sizeof(path), "/Relais%d_%s.csv", num + 1, monthNames[tm.tm_mon + 1]);
      SPIFFS.remove(path);                                          // Löscht die elf Monate alte Logdatei.
    }
    File f = SPIFFS.open(fileName, "a");                            // Die Ereignisdaten für den aktuellen Monat speichern
    if (f && freeSpace(40)) {
      relState[num] == aktiv ? f.printf("%d.;%s;%s;", tm.tm_mday, localTime(), customer.c_str()) : f.printf("%s;%s;\n", localTime(), customer.c_str());
    }
    f.close();
  }
  lastrelState[num] = relState[num];                                // Ersetzt den letzten Status durch den aktuellen Relais Status.
}
