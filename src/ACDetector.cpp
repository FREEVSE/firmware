#include <Arduino.h>
#include <ACDetector.h>
#include <Configuration.h>

void ACDetector::Init(){
    pinMode(SENS_L1_PIN, INPUT_PULLUP);
    pinMode(SENS_L2_PIN, INPUT_PULLUP);
}