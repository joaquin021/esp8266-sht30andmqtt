String ssid = "";
String password = "";

String WIFI_STATUS [] = {"WL_IDLE_STATUS", "WL_NO_SSID_AVAIL", "", "WL_CONNECTED", "WL_CONNECT_FAILED", "", "WL_DISCONNECTED"};

void connectWiFi_STA() {
  Serial.println("");
  Serial.println("Connecting to " + ssid);
  WiFi.disconnect();
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {
    Serial.print("STA running:\t");
    Serial.println(ssid);
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Connection with " + ssid + " failed");
    ssid = "";
    password = "";
  }
}

void connectWiFi_AP() {
  Serial.println("");
  Serial.println("Creating AP: ");
  const char* ssidAP = "ESP_SHT_CONFIG";
  IPAddress ip(192, 168, 2, 1);
  IPAddress gateway(192, 168, 2, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(ip, gateway, subnet);
  while (!WiFi.softAP(ssidAP)) {
    Serial.println(".");
    delay(100);
  }
  Serial.print("AP running:\t");
  Serial.println(ssidAP);
  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());
}

void setWiFiConfig() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.setAutoConnect(false);
  WiFi.printDiag(Serial);
}

boolean existWifiConfig() {
  return LittleFS.exists("/config/wifi");
}

void loadWifiConfig() {
  File configWifi = LittleFS.open("/config/wifi", "r");
  ssid = configWifi.readStringUntil('\n');
  ssid.remove(ssid.length() - 1);
  password = configWifi.readStringUntil('\n');
  password.remove(password.length() - 1);
  configWifi.close();
}

void writeWifiConfig() {
  File configWifi = LittleFS.open("/config/wifi", "w+");
  configWifi.println(ssid);
  configWifi.println(password);
  configWifi.close();
}
