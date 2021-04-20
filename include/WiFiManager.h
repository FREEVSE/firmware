#pragma once
#include <WiFi.h>
#include <semver.h>
#include <ArduinoJson.h>

//#define WIFI_TAG "WiFi Manager"
#define WIFI_INFO(fmt, ...)   ESP_LOGI(WIFI_TAG, fmt, ##__VA_ARGS__) 
#define WIFI_ERROR(fmt, ...)  ESP_LOGE(WIFI_TAG, fmt, ##__VA_ARGS__) 

enum UpdateCheckResult{
    Error,
    Latest,
    Available
};

/**
 * @brief The result of a check for an update
 * @details
 * - If the check failed, the first value will be UpdateCheckResult.Error
 * - If the check succeeded but there is no update available, the first value will be UpdateCheckResult.Latest
 * - If the check succeeded and there is an update available, the first value will be UpdateCheckResult.Available.
 * In this case, the URI of the update file will be stored in the second value.
 */
typedef std::tuple<UpdateCheckResult, const char*> UpdateCheck;

class WiFiManager{
    public:
        /**
         * @brief Initilizes the WiFiManager functionality
         */
        static void Init();

        static std::atomic<bool> IsUpdateAvailable;
        static bool IsUpdatePending;
        static char* UpdateURI;

        /**
         * @brief Checks for an update from the configured backend server
         * 
         * @return UpdateCheck The result of the check
         */
        static UpdateCheck CheckUpdate();

        /**
         * @brief Attemps to apply an update from an HTTPS source
         *
         * @param bin The URI of the binary to download
         * @return esp_err_t 
         * - ESP_OK if update succeeds
         * - Other if the update failed
         */
        static esp_err_t DoUpdate(const char* bin);
    private:
        static esp_err_t ipEventHandler(void *ctx, system_event_t *event);
        static void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info);
        static void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
        static void UpdateMonitorTask(void* param);  
        static esp_err_t InitMdns();
        static semver_t currentVersion;
        static EventGroupHandle_t wifiEvtGrp;
        static TaskHandle_t updateTask;
};