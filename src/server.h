/*
  To upload through terminal you can use: curl -F "image=@esp8266-ds18b20.cpp.generic.bin" temperature.local/update
*/
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
}

void handleStatus() {
  StaticJsonBuffer<2000> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  root["hostname"] = HOSTNAME;
  root["repository"] = REPOSITORY;

  JsonObject& settings = root.createNestedObject("settings");

  settings["INTERVAL"] = INTERVAL;
  settings["ONE_WIRE_BUS"] = ONE_WIRE_BUS;
  settings["PHANT_HOST"] = PHANT_HOST;
  settings["PHANT_STREAMID"] = PHANT_STREAMID;
  settings["PHANT_PRIVATEKEY"] = PHANT_PRIVATEKEY;

  String temp;
  root.printTo(temp);
  server.send(200, "application/json", temp);
}

void handleGet() {

  StaticJsonBuffer<2000> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  getCurrentState(root);

  // Print.
  String temp;
  root.printTo(temp);
  server.send(200, "application/json", temp);
}
