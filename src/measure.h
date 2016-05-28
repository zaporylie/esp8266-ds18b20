#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Phant.h>

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

long previousMillis = 0;

/**
 * Device address as a string.
 */
String printAddress(DeviceAddress deviceAddress)
{
  String address = "";
  for(uint8_t i = 0; i < 8; i++)
  {
    // if (i > 0) address += ":";
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) address += 0;
    address += String(deviceAddress[i], HEX);
  }
  Serial.print("Address: ");
  Serial.println(address);
  return address;
}

/**
 * Get temperatures and return as a JSON object.
 */
void getCurrentState(JsonObject& root) {

  sensors.begin();
  Serial.println(sensors.getDeviceCount());
  sensors.requestTemperatures();

  for(int index = 0; index < sensors.getDeviceCount(); index++) {
    DeviceAddress tempDeviceAddress;
    if (sensors.getAddress(tempDeviceAddress, index)) {
      float tempTemp = sensors.getTempC(tempDeviceAddress);
      Serial.print("Temp: ");
      Serial.println(tempTemp);
      root.set(printAddress(tempDeviceAddress), tempTemp);
    }
  }
}
