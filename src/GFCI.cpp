#include <GFCI.h>
#include <Configuration.h>
//#include <Arduino.h>

void GFCI::Init(){
    #ifdef EN_GFCI
    pinMode(GFI_SET_PIN, OUTPUT);
    pinMode(GFI_INT_PIN, INPUT);

    ledcSetup(GFCI_PWM_CHAN, GFCI_TST_FREQ, 8);
    ledcAttachPin(GFI_TEST_PIN, GFCI_PWM_CHAN);
    #endif
}

void GFCI::BeginTest(){
    #ifdef EN_GFCI
    ledcWrite(GFCI_PWM_CHAN, 128);
    #endif
}

void GFCI::EndTest(){
    #ifdef EN_GFCI
    ledcWrite(GFCI_PWM_CHAN, 0);
    #endif
}

bool GFCI::SelfTest(){
    #ifdef EN_GFCI
    BeginTest();
    delay(2500);
    EndTest();
    

    if(State() != false){
        return false;
    }
    delay(2500);
    Reset();

    return State();
    
    #else
    return true;
    #endif
}

void GFCI::Reset(){
    #ifdef EN_GFCI
    digitalWrite(GFI_SET_PIN, HIGH);
    delay(10);
    digitalWrite(GFI_SET_PIN, LOW);
    #endif
}

bool GFCI::State(){
    #ifdef EN_GFCI
    return digitalRead(GFI_INT_PIN);
    #else
    return true;
    #endif
}