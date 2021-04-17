#include "Arduino.h"
#include <Configuration.h>
#include <Contactor.h>
#include <ACDetector.h>

void Contactor::Init(){
    #ifdef EN_CONTACTOR
    pinMode(CTR_ENER_PIN, OUTPUT);
    #endif
}

void Contactor::Open(){
    #ifdef EN_CONTACTOR
    digitalWrite(CTR_ENER_PIN, LOW);
    #endif
}

void Contactor::Close(){
    #ifdef EN_CONTACTOR
    digitalWrite(CTR_ENER_PIN, HIGH);
    #endif
}

bool Contactor::IsClosed(){
    #ifdef EN_CONTACTOR
    return ACDetector::IsL1Present();
    #else
    return false;
    #endif
    
}