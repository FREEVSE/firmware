#include <Arduino.h>
#include <ACDetector.h>
#include <Configuration.h>
#include <driver/gpio.h>

#define PIN_MASK ((1ULL<<SENS_L1_PIN) | (1ULL<<SENS_L2_PIN))

bool ACDetector::L1Detected = false;
bool ACDetector::L2Detected = false;

void ACDetector::Init(){
    gpio_config_t conf{
        .pin_bit_mask = PIN_MASK,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_POSEDGE
    };

    gpio_config(&conf);
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    gpio_isr_handler_add((gpio_num_t)SENS_L1_PIN, DetectISR, (void *) SENS_L1_PIN);
    gpio_isr_handler_add((gpio_num_t)SENS_L2_PIN, DetectISR, (void *) SENS_L2_PIN);
    
}

void IRAM_ATTR ACDetector::DetectISR(void * arg){
    Serial.println((int)arg);
}