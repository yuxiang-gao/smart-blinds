#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <DNSServer.h>
#include <WiFiManager.h>

#include <Espalexa.h>

#include "blinds.h"

static const int SERVO_PIN = 14; //D5 GPIO14
static const char *DEVICE_NAME = "blinds 1";

Blinds *blinds;

Espalexa espalexa;
EspalexaDevice *blinds_device;

bool first_on = true;

void blindCallback(EspalexaDevice *dev);
void calibrateCallback(EspalexaDevice *dev);

void setup()
{
  Serial.begin(9600);

  blinds = new Blinds(DEVICE_NAME, SERVO_PIN, 0, 60);

  // wifiManage
  WiFiManager wifiManager;
  wifiManager.autoConnect();

  // Connected!
  Serial.printf("[WIFI] Connected. SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

  blinds_device = new EspalexaDevice(DEVICE_NAME, blindCallback, EspalexaDeviceType::dimmable);
  espalexa.addDevice(blinds_device);
  blinds_device->setState(true);
  blinds_device->setPercent(0);
  espalexa.addDevice("calibrate", calibrateCallback, EspalexaDeviceType::dimmable);
  espalexa.begin();
}

void loop()
{
  espalexa.loop();
  delay(1);
}

void blindCallback(EspalexaDevice *dev)
{
  if (dev == nullptr)
    return;
  int desired_percentage = dev->getPercent();
  if (first_on && (uint8_t)dev->getLastChangedProperty() == 1) // The first time to switch on in the alexa app, a value of 100 will be passed.
  {
    Serial.println("Switch on.");
    first_on = false;
    return;
  }
  first_on = false;
  Serial.printf("[MAIN] Device %s state: %d, value: %d%%\n", dev->getName().c_str(), dev->getState(), desired_percentage);
  blinds->moveTo(desired_percentage);
}

void calibrateCallback(EspalexaDevice *dev)
{
  if (dev == nullptr)
    return;
  uint8_t calibrate_percentage = dev->getPercent();
  Serial.printf("[MAIN] Calibrate %s  value: %d%%\n", blinds_device->getName().c_str(), calibrate_percentage);
  blinds->calibrate(calibrate_percentage);
  blinds_device->setPercent(calibrate_percentage);
}