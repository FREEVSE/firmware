#pragma once
#include <WiFi.h>
#include <Configuration.h>

#include "esp_wifi.h"
#include "semver.h"

#define WIFI_TAG "WiFi Manager"
#define WIFI_INFO(fmt, ...)   ESP_LOGI(WIFI_TAG, fmt, ##__VA_ARGS__) 
#define WIFI_ERROR(fmt, ...)  ESP_LOGE(WIFI_TAG, fmt, ##__VA_ARGS__) 

class WiFiManager{
    public:
        static void Init();
        static void Connect();
    private:
        static esp_err_t ipEventHandler(void *ctx, system_event_t *event);
        static void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info);
        static void UpdateMonitorTask(void* param);
        static semver_t currentVersion;
        static EventGroupHandle_t wifiEvtGrp;
};