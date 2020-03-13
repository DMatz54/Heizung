String zeile, zeile2;
void setupAnlage() {
  server.on("/anlage", HTTP_GET, []() {
Serial.print("Datei? ");
  File file = SPIFFS.open("/kreise.txt", "r");
  if (file.available()) {                                                          // Einlesen aller Daten falls die Datei im Spiffs vorhanden ist.
    zeile = file.readString();
    Serial.println(zeile);
  }
  file.close();
  file = SPIFFS.open("/solltemp.txt", "r");
  if (file.available()) {                                                          // Einlesen aller Daten falls die Datei im Spiffs vorhanden ist.
    zeile2 = file.readString();
    Serial.println(zeile2);
  }
  file.close();
    server.send(200, "application/json", "[" + zeile + "," + zeile2 +"]");
  });

}
