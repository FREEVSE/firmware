#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <Configuration.h>
#include <ArduinoJson.h>
//#include <semver.h>
#include <string>
#include <sstream>
#include <atomic>

#include "esp_https_ota.h"

semver_t WiFiManager::currentVersion;
EventGroupHandle_t WiFiManager::wifiEvtGrp;
TaskHandle_t WiFiManager::updateTask;

std::atomic<bool> WiFiManager::IsUpdateAvailable(false);
bool WiFiManager::IsUpdatePending = false;
char* WiFiManager::UpdateURI;

//This is lets encrypt's root ca cert.
const char *caCert = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIEkjCCA3qgAwIBAgIQCgFBQgAAAVOFc2oLheynCDANBgkqhkiG9w0BAQsFADA/\n" \
"MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
"DkRTVCBSb290IENBIFgzMB4XDTE2MDMxNzE2NDA0NloXDTIxMDMxNzE2NDA0Nlow\n" \
"SjELMAkGA1UEBhMCVVMxFjAUBgNVBAoTDUxldCdzIEVuY3J5cHQxIzAhBgNVBAMT\n" \
"GkxldCdzIEVuY3J5cHQgQXV0aG9yaXR5IFgzMIIBIjANBgkqhkiG9w0BAQEFAAOC\n" \
"AQ8AMIIBCgKCAQEAnNMM8FrlLke3cl03g7NoYzDq1zUmGSXhvb418XCSL7e4S0EF\n" \
"q6meNQhY7LEqxGiHC6PjdeTm86dicbp5gWAf15Gan/PQeGdxyGkOlZHP/uaZ6WA8\n" \
"SMx+yk13EiSdRxta67nsHjcAHJyse6cF6s5K671B5TaYucv9bTyWaN8jKkKQDIZ0\n" \
"Z8h/pZq4UmEUEz9l6YKHy9v6Dlb2honzhT+Xhq+w3Brvaw2VFn3EK6BlspkENnWA\n" \
"a6xK8xuQSXgvopZPKiAlKQTGdMDQMc2PMTiVFrqoM7hD8bEfwzB/onkxEz0tNvjj\n" \
"/PIzark5McWvxI0NHWQWM6r6hCm21AvA2H3DkwIDAQABo4IBfTCCAXkwEgYDVR0T\n" \
"AQH/BAgwBgEB/wIBADAOBgNVHQ8BAf8EBAMCAYYwfwYIKwYBBQUHAQEEczBxMDIG\n" \
"CCsGAQUFBzABhiZodHRwOi8vaXNyZy50cnVzdGlkLm9jc3AuaWRlbnRydXN0LmNv\n" \
"bTA7BggrBgEFBQcwAoYvaHR0cDovL2FwcHMuaWRlbnRydXN0LmNvbS9yb290cy9k\n" \
"c3Ryb290Y2F4My5wN2MwHwYDVR0jBBgwFoAUxKexpHsscfrb4UuQdf/EFWCFiRAw\n" \
"VAYDVR0gBE0wSzAIBgZngQwBAgEwPwYLKwYBBAGC3xMBAQEwMDAuBggrBgEFBQcC\n" \
"ARYiaHR0cDovL2Nwcy5yb290LXgxLmxldHNlbmNyeXB0Lm9yZzA8BgNVHR8ENTAz\n" \
"MDGgL6AthitodHRwOi8vY3JsLmlkZW50cnVzdC5jb20vRFNUUk9PVENBWDNDUkwu\n" \
"Y3JsMB0GA1UdDgQWBBSoSmpjBH3duubRObemRWXv86jsoTANBgkqhkiG9w0BAQsF\n" \
"AAOCAQEA3TPXEfNjWDjdGBX7CVW+dla5cEilaUcne8IkCJLxWh9KEik3JHRRHGJo\n" \
"uM2VcGfl96S8TihRzZvoroed6ti6WqEBmtzw3Wodatg+VyOeph4EYpr/1wXKtx8/\n" \
"wApIvJSwtmVi4MFU5aMqrSDE6ea73Mj2tcMyo5jMd6jmeWUHK8so/joWUoHOUgwu\n" \
"X4Po1QYz+3dszkDqMp4fklxBwXRsW10KXzPMTZ+sOPAveyxindmjkW8lGy+QsRlG\n" \
"PfZ+G6Z6h7mjem0Y+iWlkYcV4PIWL1iwBi8saCbGS5jN2p8M+X+Q7UNKEkROb3N6\n" \
"KOqkqm57TH2H3eDJAkSnh6/DNFu0Qg==\n" \
"-----END CERTIFICATE-----\n";

void WiFiManager::Init(){
    WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_STA_GOT_IP);
    WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);
    WiFi.begin();

    if(semver_parse(FREEVSE_VERSION, &currentVersion)){
        WIFI_ERROR("%s Invalid version string %s, won't be able to check for updates.", __func__, FREEVSE_VERSION);
    }
}

void WiFiManager::WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    xTaskCreate(UpdateMonitorTask, "updateMonitorTask", 5120, NULL, 1, &updateTask);
}

void WiFiManager::WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    if(updateTask != NULL){
        vTaskDelete(updateTask);
        updateTask = NULL;
    }   
}

void WiFiManager::UpdateMonitorTask(void* param){
        WIFI_INFO("Update monitoring task started.");

        while(1){
            WIFI_INFO("Time to check for an update!");
            auto res = CheckUpdate();

            if(std::get<0>(res)){
                WIFI_INFO("Update available! Won't check again.");

                IsUpdateAvailable.store(true);

                updateTask = NULL;
                vTaskDelete(NULL);
            }

            vTaskDelay(/*4.32e+7*/ 30000 / portTICK_PERIOD_MS);
        }
}

UpdateCheck WiFiManager::CheckUpdate(){
    WiFiClientSecure client;
    StaticJsonDocument<133> doc;
    semver_t availableVersion = {};

    client.setCACert(caCert);

    if (client.connect(FREEVSE_SERVER, 443)){

        WIFI_INFO("Connected to update server at %s", FREEVSE_SERVER);

        //Send the HTTP request to get JSON update info
        WIFI_INFO("GET /updates?hwv=%s&fwv=%s HTTP/1.0", FREEVSE_BOARD, FREEVSE_VERSION);
        client.printf("GET /updates?hwv=%s&fwv=%s HTTP/1.0", FREEVSE_BOARD, FREEVSE_VERSION);
        client.println();
        client.println("Host: freevse.org");
        client.println("Connection: close");
        client.println();

        //We dont care about the headers so we can just loop until they are done
        while (client.connected())
        {
            String line = client.readStringUntil('\n');
            if (line == "\r")
            {
                WIFI_INFO("Headers received");
                break;
            }
        }

        //Try to deserialize the JSON
        if (deserializeJson(doc, client))
        {
            WIFI_ERROR("Failed to parse json reply.");
            return UpdateCheck(Error, NULL);
        }

        //Parse the version string so we can see if we want it
        if (semver_parse(doc["version"], &availableVersion))
        {
            WIFI_ERROR("Invalid version string received in update check. Not updating...");
            return UpdateCheck(Error, NULL);
        }

        //Confirm that the provided binary version is in fact newer
        if (semver_compare(availableVersion, currentVersion))
        {
            auto res = UpdateCheck(Available, new char[24]);
            strcpy((char*)std::get<1>(res), doc["file"]);
            WIFI_INFO("Update available: %s", std::get<1>(res));
            return res;
        }
        else
        {
            WIFI_INFO("Latest version already running.");
            return UpdateCheck(Latest, NULL);
        }
        
    }
    else
    {
        WIFI_ERROR("Unable to connect to update server");
        return UpdateCheck(Error, NULL);
    }
}

esp_err_t WiFiManager::DoUpdate(const char* bin){
    //esp_log_level_set("*", ESP_LOG_ERROR); //Required as per 

    int len = snprintf(NULL, 0, "https://%s/updates/%s", FREEVSE_SERVER, bin);
    char* uri = (char*) malloc(len + 1);
    snprintf(uri, len+1, "https://%s/updates/%s", FREEVSE_SERVER, bin);

    WIFI_INFO("Requesting binary from: %s", uri);

    esp_http_client_config_t cfg = {
        .url = uri,
        .cert_pem = caCert
    };

    WIFI_INFO("Starting firmware update: %s", uri);

    btStop();

    disableCore1WDT();
    esp_err_t ret = esp_https_ota(&cfg);
    enableCore1WDT();

    if (ret == ESP_OK) {
        WIFI_INFO("Firmware updated! Asking for restart...");
        IsUpdatePending = true;
    } else {
        return ESP_FAIL;
    }
    return ESP_OK;
}