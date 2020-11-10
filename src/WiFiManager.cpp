#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <Configuration.h>
#include <ArduinoJson.h>
#include <semver.h>
#include <string>

#include "esp_https_ota.h"

semver_t WiFiManager::currentVersion;
EventGroupHandle_t WiFiManager::wifiEvtGrp;
TaskHandle_t WiFiManager::updateTask;

bool WiFiManager::IsUpdatePending = false;

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

//This is howsmyssl's root cert
/*const char* caCert= \
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
     "-----END CERTIFICATE-----\n";*/

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

        WiFiClientSecure client;
        StaticJsonDocument<256> doc;
        JsonObject obj;
        DeserializationError err;
        semver_t availableVersion = {};
        int res;
        char *data;

        client.setCACert(caCert);

        while(1){
            WIFI_INFO("Time to check for an update!");
            if (client.connect(FREEVSE_SERVER, 443)){
                
                WIFI_INFO("Connected to update server at %s", FREEVSE_SERVER);
                // Make a HTTP request:
                client.println("GET https://freevse.org/updates/1.0.0/1.0.0 HTTP/1.0");
                client.println("Host: freevse.org");
                client.println("Connection: close");
                client.println();

            while (client.connected()) {
                String line = client.readStringUntil('\n');
                if (line == "\r") {
                    WIFI_INFO("Headers received");
                    break;
                }
            }
            //TODO change this to static object in future, and calculate an appropriate size

            err = deserializeJson(doc, client);

            if(err){ WIFI_ERROR("Failed to parse json %s", err.c_str()); }

            //TODO remove this when API is updated to return only one binary
            obj = doc[0];

            if(semver_parse(obj["version"], &availableVersion)){
                WIFI_ERROR("Invalid version string received in update check. Not updating...");
            }

            res = semver_compare(availableVersion, currentVersion);

            if(res == 1){
                WIFI_INFO("Update available!");
                char* uri = "https://freevse.org/updates/freevse_1.2.4.bin";
                if(DoUpdate(uri) == ESP_OK){
                    updateTask = NULL;
                    vTaskDelete(NULL);
                }
            }
            else{
                WIFI_INFO("Latest version already running.");
            }

            client.stop();

            }
            else {
                WIFI_ERROR("Unable to connect to update server");
            }

            vTaskDelay(/*4.32e+7*/ 30000 / portTICK_PERIOD_MS);
        }

}

esp_err_t WiFiManager::DoUpdate(const char* uri){
    //esp_log_level_set("*", ESP_LOG_ERROR); //Required as per 

    esp_http_client_config_t cfg = {
        .url = uri,
        .cert_pem = caCert
    };

    WIFI_INFO("Starting firmware update...");

    btStop();

    disableCore0WDT();
    esp_err_t ret = esp_https_ota(&cfg);
    enableCore0WDT();

    if (ret == ESP_OK) {
        WIFI_INFO("Firmware updated! Asking for restart...");
        IsUpdatePending = true;
    } else {
        return ESP_FAIL;
    }
    return ESP_OK;
}