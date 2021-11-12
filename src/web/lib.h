const char HOME_JS[] PROGMEM = R"=====(
function getWifiConfig() {
    var xmlhttp = new XMLHttpRequest();

    xmlhttp.onreadystatechange = function() {
        if (xmlhttp.readyState == XMLHttpRequest.DONE) {
            if (xmlhttp.status == 200) {
                var response = JSON.parse(xmlhttp.responseText);
                document.getElementById("wifiSSID").innerHTML = response.ssid;
                document.getElementById("wifiIP").innerHTML = response.ip;
                document.getElementById("wifiConfig").style.display = "block";
            } else {
                console.log('error', xmlhttp);
                document.getElementById("wifiConfig").style.display = "none";
            }
        }
    };

    xmlhttp.open("GET", "getWifiConfig", true);
    xmlhttp.send();
}

function getMqttConfig() {
    var xmlhttp = new XMLHttpRequest();

    xmlhttp.onreadystatechange = function() {
        if (xmlhttp.readyState == XMLHttpRequest.DONE) {
            if (xmlhttp.status == 200) {
                var response = JSON.parse(xmlhttp.responseText);
                document.getElementById("mqttServer").innerHTML = response.server;
                document.getElementById("mqttPort").innerHTML = response.port;
                document.getElementById("mqttClientId").innerHTML = response.clientId;
                document.getElementById("mqttUser").innerHTML = response.user;
                document.getElementById("mqttConfig").style.display = "block";
            } else {
                console.log('error', xmlhttp);
                document.getElementById("mqttConfig").style.display = "none";
            }
        }
    };

    xmlhttp.open("GET", "getMqttConfig", true);
    xmlhttp.send();
}

function getShtConfig() {
    var xmlhttp = new XMLHttpRequest();

    xmlhttp.onreadystatechange = function() {
        if (xmlhttp.readyState == XMLHttpRequest.DONE) {
            if (xmlhttp.status == 200) {
                var response = JSON.parse(xmlhttp.responseText);
                document.getElementById("topicTemperature").innerHTML = response.topicTemperature;
                document.getElementById("topicHumidity").innerHTML = response.topicHumidity;
                document.getElementById("temperatureSensor").innerHTML = response.temperature;
                document.getElementById("humiditySensor").innerHTML = response.humidity;
            } 
        }
    };

    xmlhttp.open("GET", "getShtConfig", true);
    xmlhttp.send();
}



function updateWifiStatusElement(wifiStatus) {
    document.getElementById('wifiStatus').innerHTML = wifiStatus;
    if (wifiStatus == "WL_CONNECTED") {
        getWifiConfig();
    } else {
        document.getElementById("wifiConfig").style.display = "none";
    }
}

function updateMqttStatusElement(mqttStatus) {
    document.getElementById('mqttStatus').innerHTML = mqttStatus;
    if (mqttStatus == "MQTT_CONNECTED") {
        getMqttConfig();
    } else {
        document.getElementById("mqttConfig").style.display = "none";
    }
}

(function scheduleAjax() {
    checkWifiStatus();
    checkMqttStatus();
    getShtConfig();
    setTimeout(scheduleAjax, 10000);
})();
)=====";

const char ESP_CONFIG_JS[] PROGMEM = R"=====(
function checkWifiStatus() {
    var xmlhttp = new XMLHttpRequest();

    xmlhttp.onreadystatechange = function() {
        if (xmlhttp.readyState == XMLHttpRequest.DONE) {
            if (xmlhttp.status == 200) {
                updateWifiStatusElement(xmlhttp.responseText);
            } else {
                console.log('error', xmlhttp);
            }
        }
    };

    xmlhttp.open("GET", "checkWifiStatus", true);
    xmlhttp.send();
}

function checkMqttStatus() {
    var xmlhttp = new XMLHttpRequest();

    xmlhttp.onreadystatechange = function() {
        if (xmlhttp.readyState == XMLHttpRequest.DONE) {
            if (xmlhttp.status == 200) {
                updateMqttStatusElement(xmlhttp.responseText);
            } else {
                console.log('error', xmlhttp);
            }
        }
    };

    xmlhttp.open("GET", "checkMqttStatus", true);
    xmlhttp.send();
}
)=====";

const char CONNECTING_WIFI_JS[] PROGMEM = R"=====(
function updateWifiStatusElement(wifiStatus) {
    document.getElementById('wifiStatus').innerHTML = wifiStatus;
    if (wifiStatus == "WL_CONNECTED") {
        window.location.replace("/");
    } else if (wifiStatus == "WL_CONNECT_FAILED" || wifiStatus == "WL_NO_SSID_AVAIL") {
        document.getElementById("progress-bar").remove();
        document.getElementById("wait-connection-message").remove();
    }
}

(function scheduleAjax() {
    checkWifiStatus();
    setTimeout(scheduleAjax, 5000);
})();
)=====";

const char CONNECTING_MQTT_JS[] PROGMEM = R"=====(
function updateMqttStatusElement(mqttStatus) {
    document.getElementById('mqttStatus').innerHTML = mqttStatus;
    if (mqttStatus == "MQTT_CONNECTED") {
        window.location.replace("/");
    } else if (mqttStatus == "MQTT_CONNECT_BAD_CREDENTIALS" || mqttStatus == "MQTT_CONNECT_UNAUTHORIZED") {
        document.getElementById("progress-bar").remove();
        document.getElementById("wait-connection-message").remove();
    }
}

(function scheduleAjax() {
    checkMqttStatus();
    setTimeout(scheduleAjax, 5000);
})();
)=====";