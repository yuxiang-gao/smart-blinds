#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <Espalexa.h>

#include "blinds.h"

#define ServoPin 14 //D5 is GPIO14

#define WIFI_SSID "Ruison"
#define WIFI_PASS "ilovejordan1314"

Blinds blind;
Espalexa espalexa;
EspalexaDevice *alexa_device;

const char *device = "blind 1";
int desired_percentage = 0;
bool blind_changed = false;

void wifiSetup();

void blindCallback(EspalexaDevice *dev);
void calibrateCallback(EspalexaDevice *dev);

void setup()
{
  Serial.begin(9600);
  Serial.println();

  blind = Blinds(device, ServoPin);
  desired_percentage = -1; // -1 when no input from alexa

  // Wifi
  wifiSetup();

  espalexa.addDevice(device, blindCallback, EspalexaDeviceType::dimmable, 0);
  espalexa.addDevice("calibrate", calibrateCallback, EspalexaDeviceType::dimmable, 0);

  espalexa.begin();
}

void loop()
{
  espalexa.loop();
  delay(1);
  if (Serial.available() > 0)
  {
    String input = Serial.readString();
    input.trim();
    if (input == "c")
      blind.calibrate(0);
    else
    {
      int state = input.toInt();
      blind.moveTo(state);
    }
  }
}

void wifiSetup()
{

  // Set WIFI module to STA mode
  WiFi.mode(WIFI_STA);

  // Connect
  Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Wait
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  // Connected!
  Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}

void blindCallback(EspalexaDevice *dev)
{
  if (dev == nullptr)
    return;
  int desired_percentage = dev->getPercent();
  Serial.printf("[MAIN] Device %s  value: %d%%\n", dev->getName().c_str(), desired_percentage);
  blind.moveTo(desired_percentage);
}

void calibrateCallback(EspalexaDevice *dev)
{
  if (dev == nullptr)
    return;
  int calibrate_percentage = dev->getPercent();
  EspalexaDevice *d = espalexa.getDevice(0);
  Serial.printf("[MAIN] Calibrate %s  value: %d%%\n", d->getName().c_str(), calibrate_percentage);
  blind.calibrate(calibrate_percentage);
  d->setPercent(calibrate_percentage);
}