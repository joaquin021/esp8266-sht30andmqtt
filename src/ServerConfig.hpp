AsyncWebServer server(80);

void handleRequestWifiConnection(AsyncWebServerRequest *request)
{
  ssid = request->arg("ssid");
  password = request->arg("password");
  WiFi.disconnect();
  request->redirect("/connecting-wifi.html");
}

void handleRequestMqttConnection(AsyncWebServerRequest *request)
{
  mqttConfig = new MqttConfig();
  mqttConfig->server = request->arg("serverMqtt");
  mqttConfig->port = atoi(request->arg("portMqtt").c_str());
  mqttConfig->clientId = request->arg("clientIdMqtt");
  mqttConfig->user = request->arg("userMqtt");
  mqttConfig->password = request->arg("passwordMqtt");
  mqttClient.disconnect();
  request->redirect("/connecting-mqtt.html");
}

void handleRequestShtConfiguration(AsyncWebServerRequest *request)
{
  topicTemperature = request->arg("topicTemperature");
  topicHumidity = request->arg("topicHumidity");
  configShtSensor();
  request->redirect("/");
}

void extendRebootPeriod()
{
  FINISH_TIME = millis() + REBOOT_PERIOD_EXTENDED;
}

void reboot()
{
  FINISH_TIME = millis() - 1000;
}

void resetEsp()
{
  LittleFS.remove("/config/wifi");
  LittleFS.remove("/config/mqtt");
  reboot();
}

void initServer()
{

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", MAIN_page); });

  server.on("/config-mqtt.html", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", CONFIG_MQTT_HTML); });

  server.on("/config-sht.html", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", CONFIG_SHT_HTML); });

  server.on("/config-wifi.html", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", CONFIG_WIFI_HTML); });

  server.on("/connecting-mqtt.html", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", CONNECTING_MQTT_HTML); });

  server.on("/connecting-wifi.html", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", CONNECTING_WIFI_HTML); });

  server.on("/reboot.html", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", REBOOT); });

  server.on("/reset.html", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", RESET); });

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              AsyncWebServerResponse *response = request->beginResponse_P(200, "image/x-icon", FAVICON, sizeof(FAVICON));
              request->send(response);
            });

  server.on("/lib/home.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "application/javascript", HOME_JS); });

  server.on("/lib/espconfig.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "application/javascript", ESP_CONFIG_JS); });

  server.on("/lib/connecting-wifi.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "application/javascript", CONNECTING_WIFI_JS); });

  server.on("/lib/connecting-mqtt.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "application/javascript", CONNECTING_MQTT_JS); });

  server.on("/asset/espconfig.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/css", ESP_CONFIG_CSS); });

  server.on("/vendor/material.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/css", MATERIAL_CSS_MIN); });

  server.on("/vendor/material.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "application/javascript", MATERIAL_JS_MIN); });

  server.on("/vendor/google-fonts.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/css", GOOGLE_FONTS); });

  server.on("/vendor/flUhRq6.woff2", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              AsyncWebServerResponse *response = request->beginResponse_P(200, "font/woff2", flUhRq6, sizeof(flUhRq6));
              request->send(response);
            });

  server.on("/vendor/KFOmC.woff2", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              AsyncWebServerResponse *response = request->beginResponse_P(200, "font/woff2", KFOmC, sizeof(KFOmC));
              request->send(response);
            });

  server.on("/setWifiConfig", HTTP_POST, handleRequestWifiConnection);

  server.on("/checkWifiStatus", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              extendRebootPeriod();
              request->send(200, "text/plain", WIFI_STATUS[WiFi.status()]);
            });

  server.on("/getWifiConfig", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              extendRebootPeriod();
              request->send(200, "application/json", "{\"ssid\": \"" + ssid + "\", \"ip\": \"" + WiFi.localIP().toString() + "\"}");
            });

  server.on("/setMqttConfig", HTTP_POST, handleRequestMqttConnection);

  server.on("/checkMqttStatus", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              extendRebootPeriod();
              int statusCode = mqttClient.state();
              request->send(200, "text/plain", statusCode < 0 ? MQTT_STATUS_NEGATIVE[statusCode * -1] : MQTT_STATUS_POSITIVE[statusCode]);
            });

  server.on("/getMqttConfig", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              extendRebootPeriod();
              request->send(200, "application/json", mqttConfig->toJson());
            });

  server.on("/setShtConfig", HTTP_POST, handleRequestShtConfiguration);

  server.on("/getShtConfig", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              extendRebootPeriod();
              request->send(200, "application/json", getShtConfigurationJson());
            });

  server.on("/reset", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              resetEsp();
              request->redirect("/");
            });

  server.on("/reboot", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              reboot();
              request->redirect("/");
            });

  server.onNotFound([](AsyncWebServerRequest *request)
                    { request->send(400, "text/plain", "Not found"); });

  server.begin();
  Serial.println("HTTP server started");
}
