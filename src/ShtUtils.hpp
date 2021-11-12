SHT3X sht30(0x45);

String topicTemperature;
String topicHumidity;

float temperature;
float humidity;

void writeShtConfig() {
  File configSht = LittleFS.open("/config/sht", "w+");
  configSht.println(topicTemperature);
  configSht.println(topicHumidity);
  configSht.close();
}

void configShtSensor() {
  writeShtConfig();
}

boolean existShtConfig() {
  return sht30.get() == 0 && LittleFS.exists("/config/sht");
}

void loadShtConfig() {
  File configSht = LittleFS.open("/config/sht", "r");
  topicTemperature = configSht.readStringUntil('\n');
  topicTemperature.remove(topicTemperature.length() - 1);
  topicHumidity = configSht.readStringUntil('\n');
  topicHumidity.remove(topicHumidity.length() - 1);
  configSht.close();
}

String getShtConfigurationJson() {
  return "{\"topicTemperature\": \"" + topicTemperature +
         "\", \"topicHumidity\": \"" + topicHumidity +
         "\",\"temperature\": \"" + temperature +
         "\",\"humidity\": \"" + humidity + "\"}";
}

void publishTemperature() {
  float t = sht30.cTemp;
  String msg = "{\"temp\": " + String(t) + "}";
  Serial.print("Publish message: ");
  Serial.println(msg);
  char temperature[6];
  dtostrf(t, 5, 2, temperature);
  mqttClient.publish(topicTemperature.c_str(), temperature);
}

void publishHumidity() {
  float h = sht30.humidity;
  String msg = "{\"humidity\": " + String(h) + "}";
  Serial.print("Publish message: ");
  Serial.println(msg);
  char humidity[6];
  dtostrf(h, 5, 2, humidity);
  mqttClient.publish(topicHumidity.c_str(), humidity);
}

void publishSensorInfoInMqtt() {
  publishTemperature();
  publishHumidity();
  publishTemperature();
  publishHumidity();
}
