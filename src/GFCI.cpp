#include <GFCI.h>
#include <Configuration.h>
//#include <Arduino.h>

void GFCI::Init(){
    pinMode(GFI_SET_PIN, OUTPUT);
    pinMode(GFI_INT_PIN, INPUT);

    ledcSetup(GFCI_PWM_CHAN, GFCI_TST_FREQ, 8);
    ledcAttachPin(GFI_TEST_PIN, GFCI_PWM_CHAN);
}

void GFCI::BeginTest(){
    ledcWrite(GFCI_PWM_CHAN, 128);
}

void GFCI::EndTest(){
    ledcWrite(GFCI_PWM_CHAN, 0);
}

bool GFCI::SelfTest(){
    #ifdef NO_SAFETY_CHECKS
    return true;
    #else
    BeginTest();
    delay(2500);
    EndTest();
    

    if(State() != false){
        return false;
    }
    delay(2500);
    Reset();

    return State();
    #endif
}

void GFCI::Reset(){
    digitalWrite(GFI_SET_PIN, HIGH);
    delay(10);
    digitalWrite(GFI_SET_PIN, LOW);
}

bool GFCI::State(){
    #ifdef NO_SAFETY_CHECKS
    return true;
    #else
    return digitalRead(GFI_INT_PIN);
    #endif
}