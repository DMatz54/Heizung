// ****************************************************************
// Sketch Esp8266 Lokalzeit Modular(Tab)
// created: Jens Fleischer, 2018-07-10
// last mod: Jens Fleischer, 2019-05-04
// For more information visit: https://fipsok.de
// ****************************************************************
// Hardware: Esp8266
// Software: Esp8266 Arduino Core 2.4.2 / 2.5.2 / 2.6.3
// Getestet auf: Nodemcu, Wemos D1 Mini Pro, Sonoff Switch, Sonoff Dual
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
// Diese Version von Lokalzeit sollte als Tab eingebunden werden.
// #include <ESP8266WebServer.h> oder #include <ESP8266WiFi.h> muss im Haupttab aufgerufen werden.
// Funktion "setupTime();" muss im setup() nach dem Verbindungsaufbau aufgerufen werden.
/**************************************************************************************/

const char* const PROGMEM ntpServer[] = {"fritz.box", "de.pool.ntp.org", "at.pool.ntp.org", "ch.pool.ntp.org", "ptbtime1.ptb.de", "europe.pool.ntp.org"};
const char* const PROGMEM monthNames[] = {"Januar", "Februar", "März", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember"};
const char* const PROGMEM monthShortNames[] = {"Jan", "Feb", "Mrz", "Apr", "Mai", "Jun", "Jul", "Aug", "Sep", "Okt", "Nov", "Dez"};

void setupTime() {
  configTime(0, 0, ntpServer[1]);                      // deinen NTP Server einstellen (von 0 - 5 aus obiger Liste)
  setenv("TZ", "CET-1CEST,M3.5.0/02,M10.5.0/03", 1);   // Zeitzone einstellen https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
  delay(250);

  server.on("/zeit", []() {
    server.send(200, "application/json",  "\"" + (String)localTime() + "\"");
  });

}

char* localTime() {
  static char buf[9];                                  // je nach Format von "strftime" eventuell anpassen
  static time_t lastsek = 0;
  time_t now = time(&now);
  localtime_r(&now, &tm);
  if (tm.tm_sec != lastsek) {
    lastsek = tm.tm_sec;
    strftime (buf, sizeof(buf), "%T", &tm);            // http://www.cplusplus.com/reference/ctime/strftime/
    if (!(time(&now) % 86400)) {                       // einmal am Tag die Zeit vom NTP Server holen o. jede Stunde "% 3600" aller zwei "% 7200"
      setupTime();
    }
  }
  return buf;
}
