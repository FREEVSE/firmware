#include <WiFiManager.h>
#include <WiFi.h>
#include <Configuration.h>


void WiFiManager::Init(){

}

void WiFiManager::Connect(){
    Serial.println("Connecting to wifi AP in EEPROM");
    delay(10);

    char *_ssid = new char[CFG_SIZE_WIFI_SSID] {'n', '\0'};
    char *_pass = new char[CFG_SIZE_WIFI_PASS] {'n', '\0'};

    int ssidLen = Configuration::GetWiFiSSID(_ssid);
    int passLen = Configuration::GetWiFiPass(_pass);

    Serial.println(_ssid);
    Serial.println(_pass);

    delay(10);

    WiFi.begin(_ssid, _pass);

    Serial.print("Connecting to WiFi");

    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Connected!");
}

