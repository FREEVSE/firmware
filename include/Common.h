#pragma once

#include "esp_log.h"

#define LOG_E(fmt, ...) ESP_LOGE(LOG_TAG, fmt, ##__VA_ARGS__)
#define LOG_W(fmt, ...) ESP_LOGW(LOG_TAG, fmt, ##__VA_ARGS__)
#define LOG_I(fmt, ...) ESP_LOGI(LOG_TAG, fmt, ##__VA_ARGS__)
#define LOG_D(fmt, ...) ESP_LOGD(LOG_TAG, fmt, ##__VA_ARGS__)
#define LOG_V(fmt, ...) ESP_LOGV(LOG_TAG, fmt, ##__VA_ARGS__)