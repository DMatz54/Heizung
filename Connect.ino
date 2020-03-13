// ****************************************************************
// Sketch Esp8266 Connect Modular(Tab) mit optischer Anzeige
// created: Jens Fleischer, 2018-04-08
// last mod: Jens Fleischer, 2019-04-14
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
// Diese Version von Connect sollte als Tab eingebunden werden.
// #include <ESP8266WebServer.h> oder #include <ESP8266WiFi.h> muss im Haupttab aufgerufen werden
// Die Funktion "Connect();" muss im Setup eingebunden werden.
/**************************************************************************************/

//#define CONFIG           // Einkommentieren wenn der ESP dem Router die IP mitteilen soll.
#define NO_SLEEP           // Auskommentieren wenn der Nodemcu den deep sleep Modus nutzt.

const char* ssid = "SD-Matz_x2";             // << kann bis zu 32 Zeichen haben
const char* password = "3701580479220888";  // << mindestens 8 Zeichen jedoch nicht länger als 64 Zeichen

#ifdef CONFIG
IPAddress staticIP(192, 168, 24, 71);      // statische IP des NodeMCU ESP8266
IPAddress gateway(192, 168, 24, 1);        // IP-Adresse des Router
IPAddress subnet(255, 255, 255, 0);         // Subnetzmaske des Netzwerkes
IPAddress dns(192, 168, 24, 1);            // DNS Server
#endif

void Connect() {      // Funktionsaufruf "Connect();" muss im Setup eingebunden werden
  byte i = 0;
  //WiFi.disconnect();      // nur erforderlich wenn Esp den AP Modus nicht verlassen will
  WiFi.persistent(false);   // auskommentieren wenn Netzwerkname oder Passwort in den Flash geschrieben werden sollen
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
#ifdef CONFIG
  WiFi.config(staticIP, gateway, subnet, dns);
#endif
  while (WiFi.status() != WL_CONNECTED) {
#ifdef NO_SLEEP
    pinMode(LED_BUILTIN, OUTPUT);     // OnBoardLed Nodemcu, Wemos D1 Mini Pro
    digitalWrite(LED_BUILTIN, 0);
#endif
    delay(500);
    digitalWrite(LED_BUILTIN, 1);
    delay(500);
    Serial.printf(" %d sek\n", ++i);
    if (i > 9) {
      Serial.print("\nVerbindung zum AP fehlgeschlagen !\n\n");
      ESP.restart();
    }
  }
  Serial.println("\nVerbunden mit: " + WiFi.SSID());
  WiFi.hostname("Heizung");
  Serial.println("Esp8266 IP: " + WiFi.localIP().toString());
}
