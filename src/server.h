/*
  To upload through terminal you can use: curl -F "image=@esp8266-ds18b20.cpp.generic.bin" temperature.local/update
*/
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WebSocketsServer.h>
#include <Hash.h>

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;
WebSocketsServer webSocket = WebSocketsServer(81);


/**
 * Publish data to Phant.
 */
void publish() {
  StaticJsonBuffer<2000> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  getCurrentState(root);

  Phant phant(PHANT_HOST, PHANT_STREAMID, PHANT_PRIVATEKEY);

  for(JsonObject::iterator it=root.begin(); it!=root.end(); ++it) {
    phant.add((String)it->key, it->value.as<float>());
  }

  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(PHANT_HOST, httpPort)) {
    Serial.println("connection failed");
    // return "Failed";
  }
  client.print(phant.post());


  // To String.
  String temp;
  root.printTo(temp);

  // send data to all connected clients
  webSocket.broadcastTXT(temp);
}

/**
 * Log based on interval.
 */
void log() {
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > INTERVAL) {
    previousMillis = currentMillis;
    publish();
  }
}

/**
 * Handle not found.
 */
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

/**
 * Handle board status.
 */
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

/**
 * Handle get requests.
 */
void handleGet() {

  StaticJsonBuffer<2000> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  getCurrentState(root);

  // Print.
  String temp;
  root.printTo(temp);
  server.send(200, "application/json", temp);
}

/**
 * Handle websocket requests.
 */
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;

    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        // send message to client
        webSocket.sendTXT(num, "ok");
      }
      break;

    case WStype_TEXT:
      {
        Serial.printf("[%u] get Text: %s\n", num, payload);

        StaticJsonBuffer<2000> jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();

        if (payload[0] == '{') {
          getCurrentState(root);
        }

        String temp;
        root.printTo(temp);
        // send data to all connected clients
        webSocket.broadcastTXT(temp);
      }
      break;
  }
}
