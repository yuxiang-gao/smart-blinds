#include <Arduino.h>
#include <fauxmoESP.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "blinds.h"

#define ServoPin 14 //D5 is GPIO14
#define LED 2
#define WIFI_SSID "Ruison"
#define WIFI_PASS "ilovejordan1314"

blinds blind;
fauxmoESP fauxmo;

String device = "blind 1";
int desired_percentage = 0;
bool blind_changed = false;

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

void setup()
{
  Serial.begin(9600);
  Serial.println();

  pinMode(LED, OUTPUT);
  blind = blinds(device, ServoPin);
  desired_percentage = -1; // -1 when no input from alexa

  // Wifi
  wifiSetup();

  // By default, fauxmoESP creates it's own webserver on the defined port
  // The TCP port must be 80 for gen3 devices (default is 1901)
  // This has to be done before the call to enable()
  fauxmo.createServer(true); // not needed, this is the default value
  fauxmo.setPort(80);        // This is required for gen3 devices

  // You have to call enable(true) once you have a WiFi connection
  // You can enable or disable the library at any moment
  // Disabling it will prevent the devices from being discovered and switched
  fauxmo.enable(true);

  fauxmo.addDevice(device.c_str());
  fauxmo.addDevice("calibrate");

  fauxmo.onSetState([](unsigned char device_id, const char *device_name, bool state, unsigned char value) {
    //      int input_value = value - '0';
    value = int(float(value) / 255 * 100);
    Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d%%\n", device_id, device_name, state ? "ON" : "OFF", value);

    if ((strcmp(device_name, "blind 1")) == 0)
    {
      desired_percentage = value;
    }
    else if (strcmp(device_name, "calibrate") == 0)
    {
      blind.calibrate(int(value));
    }
  });
}

void loop()
{
  fauxmo.handle();
  if (desired_percentage != -1)
  {
    blind.move_to(desired_percentage);
    desired_percentage = -1;
  }
  if (Serial.available() > 0)
  {
    String input = Serial.readString();
    input.trim();
    if (input == "c")
      blind.calibrate(0);
    else
    {
      int state = input.toInt();
      blind.move_to(state);
    }
  }
}
