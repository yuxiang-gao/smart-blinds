#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <DNSServer.h>
#include <WiFiManager.h>

#include <PubSubClient.h>

#include "blinds.h"

static const int SERVO_PIN = 14; //D5 GPIO14
static const char *DEVICE_NAME = "blinds 1";
static const char *CMD_TOPIC = "bedroom/blinds/1";
static const char *STATE_TOPIC = "bedroom/blinds/1/state";

Blinds *blinds;

// MQTT client
WiFiClient espClient;
PubSubClient client(espClient);
const char *mqtt_server = "192.168.0.11";

// void blindCallback(EspalexaDevice *dev);
// void calibrateCallback(EspalexaDevice *dev);
void callback(char *topic, byte *payload, unsigned int length);
void pubMsg(const char *topic, int value)
{
  char buf[4];
  itoa(value, buf, 10);
  client.publish(topic, buf);
}

int decodeMsg(byte *payload, unsigned int length)
{
  payload[length] = '\0';
  String str = String((char *)payload);
  return str.toInt();
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient"))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      pubMsg(STATE_TOPIC, blinds->getState());
      // ... and resubscribe
      client.subscribe(CMD_TOPIC);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(9600);

  blinds = new Blinds(DEVICE_NAME, SERVO_PIN, 0, 60);

  // wifiManage
  WiFiManager wifiManager;
  wifiManager.autoConnect("SmartBlindsAP", "12345678");

  // Connected!
  Serial.printf("[WIFI] Connected. SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  delay(1500);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}

// Callback function
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.printf("t: %s, p: %s, l: %d \n", topic, payload, length);
  if (strcmp(topic, CMD_TOPIC) == 0)
  {
    int desired_percentage = decodeMsg(payload, length);
    Serial.printf("[MAIN] Device: %s, value: %d%%\n", topic, desired_percentage);
    blinds->moveTo(desired_percentage);

    pubMsg(STATE_TOPIC, desired_percentage);
  }
}