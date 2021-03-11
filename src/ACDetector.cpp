#include <Arduino.h>
#include <ACDetector.h>
#include <Configuration.h>
#include <driver/gpio.h>

#define PIN_MASK ((1ULL<<SENS_L1_PIN) | (1ULL<<SENS_L2_PIN))

bool ACDetector::L1Detected = false;
bool ACDetector::L2Detected = false;
volatile unsigned long ACDetector::lastL1Detection = 0;
volatile unsigned long ACDetector::lastL2Detection = 0;

void ACDetector::Init(){
    #ifdef EN_AC_DETECT
    gpio_config_t conf{
        .pin_bit_mask = PIN_MASK,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE
    };

    gpio_config(&conf);
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    gpio_isr_handler_add((gpio_num_t)SENS_L1_PIN, DetectISR, (void *) SENS_L1_PIN);
    gpio_isr_handler_add((gpio_num_t)SENS_L2_PIN, DetectISR, (void *) SENS_L2_PIN);
    #endif
}

bool ACDetector::IsL1Present(){
    #ifdef EN_AC_DETECT
    return lastL1Detection > micros() - 9000 || digitalRead(SENS_L1_PIN) == LOW;
    #else
    return true;
    #endif
}

bool ACDetector::IsL2Present(){
    #ifdef EN_AC_DETECT
    return lastL2Detection > micros() - 9000 || digitalRead(SENS_L2_PIN) == LOW;
    #else
    return false;
    #endif
}

void IRAM_ATTR ACDetector::DetectISR(void * arg){
    if((int) arg == SENS_L1_PIN){
        lastL1Detection = micros();
    }
    else {
        lastL2Detection = micros();
    }
}