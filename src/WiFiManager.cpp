#include <WiFiManager.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Configuration.h>


void WiFiManager::Init(){

}

void WiFiManager::Connect(){
    Serial.println("Connecting to wifi AP in EEPROM");

    String ssid = Configuration::GetWiFiSSID();
    String pass = Configuration::GetWiFiPass();

    WiFi.begin(ssid.c_str(), pass.c_str());

    Serial.print("Connecting to WiFi");

    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Connected!");
}

