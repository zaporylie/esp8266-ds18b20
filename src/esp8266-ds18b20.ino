#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <WiFiManager.h> 
#include <ESP8266mDNS.h>
#include "settings.h"
#include "measure.h"
#include "server.h"

void setup() {
  Serial.begin(115200);

  WiFiManager wifi;
  wifi.autoConnect(HOSTNAME);

  if ( MDNS.begin(HOSTNAME) ) {
    Serial.println("MDNS responder started");
  }

  server.on ( "/", HTTP_GET, handleStatus );
  server.on ( "/espstack/status", HTTP_GET, handleStatus );
  server.on ( "/api", HTTP_GET, handleGet );
  server.onNotFound ( handleNotFound );
  //httpUpdater.setup(&server);
  server.begin();
  Serial.println ( "HTTP server started" );
  
  sensors.begin();
}

void loop() {
  server.handleClient();
  log();
}
