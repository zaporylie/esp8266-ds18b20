#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
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

  server.begin();
  Serial.println ( "HTTP server started" );

  sensors.begin();

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
    ESP.restart();
  });
  ArduinoOTA.begin();
}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();
  log();
}
