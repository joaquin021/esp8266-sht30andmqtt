String MQTT_STATUS_POSITIVE [] = {"MQTT_CONNECTED", "MQTT_CONNECT_BAD_PROTOCOL", "MQTT_CONNECT_BAD_CLIENT_ID", "MQTT_CONNECT_UNAVAILABLE", "MQTT_CONNECT_BAD_CREDENTIALS", "MQTT_CONNECT_UNAUTHORIZED"};
String MQTT_STATUS_NEGATIVE [] = {"", "MQTT_DISCONNECTED", "MQTT_CONNECT_FAILED", "MQTT_CONNECTION_LOST", "MQTT_CONNECTION_TIMEOUT"};

class MqttConfig {
  public:
    String server;
    int port;
    String clientId;
    String user;
    String password;

    String toJson() {
      return String("{\"server\": \"") + server +
             "\", \"port\": \"" + port +
             "\",\"clientId\": \"" + clientId +
             "\",\"user\": \"" + user + "\"}";
    }
};

WiFiClient espClient;
PubSubClient mqttClient(espClient);
MqttConfig *mqttConfig = NULL;

void connectToMqttServer() {
  Serial.println("Attempting MQTT connection...");
  mqttClient.setServer(mqttConfig->server.c_str(), mqttConfig->port);
  for (int i = 0; i < 5 && !mqttClient.connected(); i++) {
    Serial.print("Attemping ");
    Serial.println(i);
    if (mqttClient.connect(mqttConfig->clientId.c_str(), mqttConfig->user.c_str(), mqttConfig->password.c_str())) {
      Serial.println("Connected to MQTT server: ");
      Serial.println(mqttConfig->toJson());
    } else {
      Serial.print("failed, rc=");
      Serial.println(mqttClient.state());
      delay(1000);
      if (i == 4) {
        delete mqttConfig;
        mqttConfig = NULL;
      }
    }
  }
}

boolean existMqttConfig() {
  return LittleFS.exists("/config/mqtt");
}

void loadMqttConfig() {
  mqttConfig = new MqttConfig();
  File configMqtt = LittleFS.open("/config/mqtt", "r");
  mqttConfig->server = configMqtt.readStringUntil('\n');
  mqttConfig->server.remove(mqttConfig->server.length() - 1);
  String port = configMqtt.readStringUntil('\n');
  port.remove(port.length() - 1);
  mqttConfig->port = atoi(port.c_str());
  mqttConfig->clientId = configMqtt.readStringUntil('\n');
  mqttConfig->clientId.remove(mqttConfig->clientId.length() - 1);
  mqttConfig->user = configMqtt.readStringUntil('\n');
  mqttConfig->user.remove(mqttConfig->user.length() - 1);
  mqttConfig->password = configMqtt.readStringUntil('\n');
  mqttConfig->password.remove(mqttConfig->password.length() - 1);
  configMqtt.close();
}

void writeMqttConfig() {
  File configMqtt = LittleFS.open("/config/mqtt", "w+");
  configMqtt.println(mqttConfig->server);
  configMqtt.println(mqttConfig->port);
  configMqtt.println(mqttConfig->clientId);
  configMqtt.println(mqttConfig->user);
  configMqtt.println(mqttConfig->password);
  configMqtt.close();
}
