#include <OneWire.h>
#include <DallasTemperature.h>
#include <Phant.h>

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


long previousMillis = 0;


// function to print a device address
String getAddress(DeviceAddress deviceAddress)
{
  String address;
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) address += 0;
    address += String(deviceAddress[i], HEX);
  }
}

JsonObject& getCurrentState() {
  StaticJsonBuffer<2000> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  for (int index = 0; index < sensors.getDeviceCount(); index++) {
    root.set((char*)index, sensors.getTempCByIndex(index));
  }
  

  return root;
}

void publish() {
  Phant phant(PHANT_HOST, PHANT_STREAMID, PHANT_PRIVATEKEY);
  
  JsonObject& root = getCurrentState();
  for (JsonObject::iterator it=root.begin(); it!=root.end(); ++it) {
    // phant.add((String)it->key, it->value.asString());
  }
    
  //phant.post();
  //phant.clear();
}

void log() {
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > INTERVAL) {
    previousMillis = currentMillis;   
    publish();
  }
}
