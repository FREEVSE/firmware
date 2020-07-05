#include <GFCI.h>
#include <Configuration.h>
#include <Arduino.h>

hw_timer_t * GFCI::intTimer;

void GFCI::Init(){
    pinMode(GFI_SET, OUTPUT);
    pinMode(GFI_INT, INPUT);

    ledcSetup(GFCI_PWM_CHAN, GFCI_TST_FREQ, 8);
    ledcAttachPin(GFI_TEST, GFCI_PWM_CHAN);

    intTimer = timerBegin(GFCI_TIMER, 80000, true);
}

void GFCI::BeginTest(){
    ledcWrite(GFCI_PWM_CHAN, 128);
}

void GFCI::EndTest(){
    ledcWrite(GFCI_PWM_CHAN, 0);
}

void GFCI::Reset(){
    digitalWrite(GFI_SET, HIGH);
    delay(10);
    digitalWrite(GFI_SET, LOW);
}

bool GFCI::State(){
    return digitalRead(GFI_INT);
}

void GFCI::Inhibit(int ms){
    Serial.println("GFCI Inhibit");
    pinMode(GFI_INT, OUTPUT);
    digitalWrite(GFI_INT, HIGH);
    
    timerRestart(intTimer);
    timerAttachInterrupt(intTimer, &EndInhibit, true);
    timerAlarmWrite(intTimer, ms, false);
    timerAlarmEnable(intTimer);
}

void IRAM_ATTR GFCI::EndInhibit(){
    Serial.println("GFCI re-enabled");
    Reset();
    //digitalWrite(GFI_INT, LOW);
    pinMode(GFI_INT, INPUT);

    timerStop(intTimer);
    timerDetachInterrupt(intTimer);
    timerAlarmDisable(intTimer);
}