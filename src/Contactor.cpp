#include "Arduino.h"
#include <Configuration.h>
#include <Contactor.h>
#include <ACDetector.h>

void Contactor::Init(){
    pinMode(CTR_ENER_PIN, OUTPUT);
}

void Contactor::Open(){
    digitalWrite(CTR_ENER_PIN, LOW);
}

void Contactor::Close(){
    digitalWrite(CTR_ENER_PIN, HIGH);
}

bool Contactor::IsClosed(){
    return ACDetector::IsL1Present();
}