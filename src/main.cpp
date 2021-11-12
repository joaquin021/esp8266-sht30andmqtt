#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <PubSubClient.h>
#include <WEMOS_SHT3X.h>

int INPUT_AP_MODE = 12;
/*
   0 CONFIG MODE MODE
   1 STANDARD MODE
*/
int BOOT_MODE = 0;

long REBOOT_PERIOD = 90000;
long REBOOT_PERIOD_EXTENDED = 300000;
unsigned FINISH_TIME;

#include "WifiUtils.hpp"
#include "MqttService.hpp"
#include "ShtUtils.hpp"
#include "web/html.h"
#include "web/asset.h"
#include "web/lib.h"
#include "web/vendor.h"
#include "ServerConfig.hpp"

void configConfigMode()
{
  pinMode(LED_BUILTIN, OUTPUT);
  connectWiFi_AP();
  initServer();
  if (existWifiConfig())
  {
    loadWifiConfig();
    connectWiFi_STA();
  }
  if (existMqttConfig())
  {
    loadMqttConfig();
    connectToMqttServer();
  }
  if (existShtConfig())
  {
    loadShtConfig();
    configShtSensor();
  }
}

void configStandardMode()
{
  loadWifiConfig();
  WiFi.softAPdisconnect(true);
  connectWiFi_STA();
  loadMqttConfig();
  connectToMqttServer();
  loadShtConfig();
  configShtSensor();
  if (WiFi.status() != WL_CONNECTED || !mqttClient.connected())
  {
    pinMode(LED_BUILTIN, OUTPUT);
    connectWiFi_AP();
    initServer();
    BOOT_MODE = 0;
  }
  else
  {
    LittleFS.end();
  }
}

void setBootMode()
{
  Serial.println("");
  if (digitalRead(INPUT_AP_MODE) == 1)
  {
    Serial.println("BOOT IN CONFIG MODE FROM PIN");
    BOOT_MODE = 0;
    configConfigMode();
  }
  else if (existWifiConfig() && existMqttConfig() && existShtConfig())
  {
    Serial.println("BOOT IN STANDARD MODE");
    BOOT_MODE = 1;
    configStandardMode();
  }
  else
  {
    Serial.println("BOOT IN CONFIG MODE");
    BOOT_MODE = 0;
    configConfigMode();
  }
}

void setup()
{
  pinMode(INPUT_AP_MODE, INPUT);
  Serial.begin(115200);
  Serial.println();
  LittleFS.begin();
  setWiFiConfig();
  setBootMode();
  FINISH_TIME = millis() + REBOOT_PERIOD;
}

void loop()
{
  switch (BOOT_MODE)
  {
  case 1:
    if (!mqttClient.connected())
    {
      connectToMqttServer();
    }
    publishSensorInfoInMqtt();
    mqttClient.disconnect();
    WiFi.disconnect();
    ESP.deepSleep(600000000);
    delay(500);
    break;
  default:
    humidity = sht30.humidity;
    temperature = sht30.cTemp;
    if (!ssid.isEmpty() && !password.isEmpty() && WiFi.status() != WL_CONNECTED)
    {
      connectWiFi_STA();
      if (WiFi.status() == WL_CONNECTED)
      {
        writeWifiConfig();
      }
    }
    if (mqttConfig != NULL && !mqttClient.connected())
    {
      connectToMqttServer();
      if (mqttClient.connected())
      {
        writeMqttConfig();
      }
    }
    if (millis() > FINISH_TIME)
    {
      Serial.println("Restarting for inactivity");
      mqttClient.disconnect();
      WiFi.disconnect();
      LittleFS.end();
      ESP.restart();
    }
    delay(2000);
    break;
  }
}
