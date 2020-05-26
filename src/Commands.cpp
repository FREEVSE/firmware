#include <Commands.h>
#include <CommandHandler.h>
#include <Configuration.h>

#include <WiFiManager.h>

void Cmd_SetWiFiSSID(CommandParameter &params){
    const char * ssid = params.NextParameter();
    Configuration::SetWiFiSSID(ssid);
    Serial.print("Set WiFi SSID to: ");
    Serial.println(ssid);
}

void Cmd_SetWiFiPass(CommandParameter &params){
    const char * pass = params.NextParameter();
    Configuration::SetWiFiPass(pass);
    Serial.print("Set WiFi password to: ");
    Serial.println(pass);
}

void Cmd_ReadSettings(CommandParameter &params){
    Serial.println("**Charge Settings**");
    Serial.print("Max amp output: ");
    Serial.println(Configuration::GetMaxOutputAmps());

    Serial.println("**WiFi Settings**");
    Serial.print("SSID: ");

    char * ssid = new char[CFG_SIZE_WIFI_SSID];
    Configuration::GetWiFiSSID(ssid);
    Serial.println(ssid);

    Serial.print("Password: ");

    char * pass = new char[CFG_SIZE_WIFI_PASS];
    Configuration::GetWiFiPass(pass);
    Serial.println(pass);
}

void Cmd_WiFiConnect(CommandParameter &params){
    WiFiManager::Connect();
}