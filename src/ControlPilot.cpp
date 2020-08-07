#include <Configuration.h>
#include <ControlPilot.h>
#include <Arduino.h>

#include <driver/timer.h>
#include <driver/gpio.h>
#include <soc/soc.h>

#define ISR_DELAY 5

#define SCHED_ACTION(t, a)\
    timerAttachInterrupt(intTimer, &a, true);\
    timerAlarmWrite(intTimer, t, true);\
    timerAlarmEnable(intTimer);

//Private
DRAM_ATTR hw_timer_t * ControlPilot::intTimer;

bool ControlPilot::pulsing = false;
int ControlPilot::highTime = 0;

int ControlPilot::lastCpValue = 0;

void ControlPilot::Init(){
    pinMode(CP_PWM_PIN, OUTPUT);
    pinMode(32, OUTPUT);
    digitalWrite(CP_PWM_PIN, LOW);
    pinMode(32, OUTPUT);
    adcAttachPin(CP_READ_PIN);
  
    ControlPilot::highTime = (int)(Configuration::GetCpPwmDutyCycle() * CP_PWM_FREQ);
    
    intTimer = timerBegin(CP_TIMER, 80, true);
}

void IRAM_ATTR ControlPilot::PulseLow(){
    digitalWrite(CP_PWM_PIN, HIGH);
    
    SCHED_ACTION(CP_PWM_FREQ - highTime, PulseHigh);
}

void IRAM_ATTR ControlPilot::PulseHigh(){
    digitalWrite(CP_PWM_PIN, LOW);

    SCHED_ACTION(30, Sample);
}

void IRAM_ATTR ControlPilot::Sample(){
    digitalWrite(32, HIGH);
    adcStart(CP_READ_PIN);
    digitalWrite(32, LOW);

    SCHED_ACTION(highTime - 30, PulseLow);
}

void ControlPilot::BeginPulse(){
    if(pulsing)
        return;

    PulseHigh();

   pulsing = true;
}

void ControlPilot::EndPulse(){
    if(!pulsing)
        return;

    timerAlarmDisable(intTimer);
    digitalWrite(CP_PWM_PIN, LOW);

    pulsing = false;
}

CpState ControlPilot::State(){
    int cpValue;

    if(pulsing){
        if(adcBusy(CP_READ_PIN))
            cpValue = ControlPilot::lastCpValue;
        else
            cpValue = adcEnd(CP_READ_PIN);
    }
    else
        cpValue = analogRead(CP_READ_PIN);
    
    ControlPilot::lastCpValue = cpValue;
    
    if(cpValue >= CP_IDLE_MIN){
        return CpState::Idle;  
    }              
    
    if(cpValue <= CP_PRESENT_MAX && cpValue >= CP_PRESENT_MIN){
        return CpState::VehicleDetected;
    }
        
    
    if(cpValue <= CP_CHARGING_MAX && cpValue >= CP_CHARGING_MIN){
        return CpState::Charge;
    }

    if(cpValue <= CP_WITH_VENT_MAX && cpValue >= CP_WITH_VENT_MIN){
        return CpState::ChargeWithVentilation;
    }

    return CpState::Invalid;
}

const char* ControlPilot::ToString(const CpState value)
{
    static const char* LUT[] = {"Idle", "VehicleDetected", "Charge", "ChargeWithVentilation", "Invalid" };
    return LUT[static_cast<int>(value)];
}
