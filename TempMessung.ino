// Werte durch Messung ersetzen
float zimmertemp[] = {19.5,20,20.5,18,22};
float wassertemp[] = {43,44,44.5,44,40,48};
//

void setupMessung() {
  sensors.begin(); //Starten der Kommunikation mit dem Sensor
  sensorCount = sensors.getDS18Count(); //Lesen der Anzahl der angeschlossenen Temperatursensoren.
  server.on("/werte2", HTTP_GET, []() {
    File file = SPIFFS.open("/ventilstellung.txt", "r");
    if (file.available()) {                                                          // Einlesen aller Daten falls die Datei im Spiffs vorhanden ist.
      zeile = file.readString();
    }
    file.close();
    String temp = "";
    for(int i = 0; i <6 ; i++) {
      temp = temp + wassertemp[i] + ",";
    }
    for(int i = 0; i <5 ; i++) {
      temp += zimmertemp[i];
      if ( i != 4) {
        temp += ",";
      }
    }
    server.send(200, "application/json", "[" + zeile + "," + temp +"]");
  });
  server.on("/werte1", HTTP_POST, []() {
    if (server.hasArg("Temp")) {
      char str[140];
      strcpy (str, server.arg("Temp").c_str());           // Liest die vom Browser übergebene Werte nach Argument 
      Serial.println(str);
      String temp = str;
//      File file = SPIFFS.open("/solltemp.txt", "w");
//      if (file) {
//        file.println(str);
//        file.close();
//      }
   }
  });
}

void loopMessung() {
 sensors.requestTemperatures(); 
 //Ausgabe aller Werte der angeschlossenen Temperatursensoren.
 /*
  * Zuweisung Senor zu Kreis Stand 12.3.2020
  * Kreis 0 => Bad Sensor 3
  * Kreis 1 => Bad Sensor 0
  * Kreis 2 => Bad Sensor 2
  * Kreis 3 => Bad Sensor 4
  * Kreis 4 => Bad Sensor 5
  * Kreis 5 => Bad Sensor 1
  */
// for(int i=0;i<sensorCount;i++){
   wassertemp[0] = sensors.getTempCByIndex(3);
   wassertemp[1] = sensors.getTempCByIndex(0);
   wassertemp[2] = sensors.getTempCByIndex(2);
   wassertemp[3] = sensors.getTempCByIndex(4);
   wassertemp[4] = sensors.getTempCByIndex(5);
   wassertemp[5] = sensors.getTempCByIndex(1);
// }
  
}
