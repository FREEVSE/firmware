#include "Bluetooth.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <WiFi.h>

#include "blufi.h"

#define SERVICE_UUID        "61686c77-920f-4867-a1fb-b8000dd29408"
#define CHARACTERISTIC_UUID "9f2c8812-4a17-4def-b8e8-1dcfff50f4c2"

void Bluetooth::Init(){

    BLEDevice::init("FREEVSE");
    //esp_ble_gap_register_callback(gap_event_handler);
    //esp_blufi_register_callbacks(&callbacks);

    //if(!WiFi.isConnected())
      Blufi::init();
      
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

    pCharacteristic->setValue("Success!");
    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
}