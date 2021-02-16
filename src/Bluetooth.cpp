#include <Common.h>
#include "Bluetooth.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <esp_bt_main.h>
#include <WiFi.h>

#include "blufi.h"

#define LOG_TAG "Bluetooth"

#define SERVICE_UUID        "61686c77-920f-4867-a1fb-b8000dd29408"
#define CHARACTERISTIC_UUID "9f2c8812-4a17-4def-b8e8-1dcfff50f4c2"

void Bluetooth::Init(){
  /*esp_err_t ret;

  esp_bt_controller_config_t btCfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

  ESP_ERROR_CHECK(esp_bt_controller_init(&btCfg));
  ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BTDM));
  ESP_ERROR_CHECK(esp_bluedroid_init());
  ESP_ERROR_CHECK(esp_bluedroid_enable());*/

  BLEDevice::init("FREEVSE");

  //if(!WiFi.isConnected())
  Blufi::init();
}