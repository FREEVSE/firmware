#pragma once

#define BLUFI_TAG "BLUFI"
#define BLUFI_INFO(fmt, ...)   ESP_LOGI(BLUFI_TAG, fmt, ##__VA_ARGS__) 
#define BLUFI_ERROR(fmt, ...)  ESP_LOGE(BLUFI_TAG, fmt, ##__VA_ARGS__) 

class Bluetooth{
    public:
        static void Init();
};
