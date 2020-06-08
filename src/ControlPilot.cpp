#include <Configuration.h>
#include <ControlPilot.h>
#include <Arduino.h>

#include <driver/ledc.h>

//Private
hw_timer_t * ControlPilot::intTimer;
bool ControlPilot::pulsing = false;

int ControlPilot::lastCpValue = 0;

void ControlPilot::Init(){
    pinMode(CP_PWM, OUTPUT);
    pinMode(32, OUTPUT);
    adcAttachPin(CP_READ);

    ledcSetup(0, CP_PWM_FREQ, 10);
    ledcAttachPin(CP_PWM, 0);
    ledcWrite(0, 0);

    delayMicroseconds(170);

    intTimer = timerBegin(0, 80, true);
    timerAttachInterrupt(intTimer, &read_cp, true);
    timerAlarmWrite(intTimer, 10000, true);
}

void IRAM_ATTR ControlPilot::read_cp(){
    digitalWrite(32, HIGH);
    adcStart(CP_READ);
    digitalWrite(32, LOW);
}

void ControlPilot::BeginPulse(){
    ledcWrite(0, Configuration::GetCpPwmDutyCycle());
    timerAlarmEnable(intTimer);

    pulsing = true;
}

void ControlPilot::EndPulse(){
    if(!pulsing)
        return;

    timerAlarmDisable(intTimer);
    ledcWrite(0, 0);

    pulsing = false;
}

CpState ControlPilot::State(){
    int cpValue;

    if(pulsing){
        if(adcBusy(CP_READ)){
            Serial.println("Adc is busy, returning last CP value");
            cpValue = ControlPilot::lastCpValue;
        }
        else{
            Serial.println("Adc not busy, retuning new value");
            cpValue = adcEnd(CP_READ);
        }
    }
    else{
        Serial.println("Not pulsing, retuning analog read value");
        cpValue = analogRead(CP_READ);
    }
    
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
