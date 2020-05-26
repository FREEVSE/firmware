#include <GFCI.h>
#include <Configuration.h>
#include <Arduino.h>

void GFCI::Init(){
    pinMode(GFI_SET, OUTPUT);
    pinMode(GFI_INT, INPUT);
}

void GFCI::Reset(){
    digitalWrite(GFI_SET, HIGH);
    delay(10);
    digitalWrite(GFI_SET, LOW);
}

bool GFCI::State(){
    return digitalRead(GFI_INT);
}