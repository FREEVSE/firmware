#include <Configuration.h>
#include <ControlPilot.h>
#include <Arduino.h>

#include <driver/ledc.h>

#define ISR_DELAY 5

//Private
hw_timer_t * ControlPilot::intTimer;
bool ControlPilot::pulsing = false;
int ControlPilot::highTime = 0;

int ControlPilot::lastCpValue = 0;

void ControlPilot::Init(){
    pinMode(CP_PWM, OUTPUT);
    digitalWrite(CP_PWM, LOW);
    pinMode(32, OUTPUT);
    adcAttachPin(CP_READ);

    ControlPilot::highTime = (int)(Configuration::GetCpPwmDutyCycle() * CP_PWM_FREQ);

    intTimer = timerBegin(CP_TIMER, 80, true);
}

void IRAM_ATTR ControlPilot::PulseHigh(){
    digitalWrite(CP_PWM, HIGH);

    timerRestart(intTimer);
    timerAttachInterrupt(intTimer, &PulseLow, true);
    timerAlarmWrite(intTimer, CP_PWM_FREQ - highTime, false);
    timerAlarmEnable(intTimer);
}

void IRAM_ATTR ControlPilot::PulseLow(){
    digitalWrite(CP_PWM, LOW);

    //delayMicroseconds(10);
    adcStart(CP_READ);

    timerRestart(intTimer);
    timerAttachInterrupt(intTimer, &PulseHigh, true);
    timerAlarmWrite(intTimer, highTime - ISR_DELAY, false);
    timerAlarmEnable(intTimer);
}

void IRAM_ATTR ControlPilot::read_cp(){
    digitalWrite(32, HIGH);
    adcStart(CP_READ);
    digitalWrite(32, LOW);
}

void ControlPilot::BeginPulse(){

    timerAttachInterrupt(intTimer, &PulseHigh, true);
    timerAlarmWrite(intTimer, 10000, false);
    timerAlarmEnable(intTimer);

    pulsing = true;
}

void ControlPilot::EndPulse(){
    if(!pulsing)
        return;

    timerAlarmDisable(intTimer);
    digitalWrite(CP_PWM, LOW);

    pulsing = false;
}

CpState ControlPilot::State(){
    int cpValue;

    if(pulsing){
        if(adcBusy(CP_READ))
            cpValue = ControlPilot::lastCpValue;
        else
            cpValue = adcEnd(CP_READ);
    }
    else
        cpValue = analogRead(CP_READ);
    
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
