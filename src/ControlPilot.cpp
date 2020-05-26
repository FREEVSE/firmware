#include <Configuration.h>
#include <ControlPilot.h>
#include <Arduino.h>

//Private
hw_timer_t * ControlPilot::intTimer;
bool ControlPilot::pulsing = false;

void ControlPilot::Init(){
    pinMode(CP_PWM, OUTPUT);
    adcAttachPin(CP_READ);

    ledcSetup(0, CP_PWM_FREQ, 10);
}

void IRAM_ATTR ControlPilot::read_cp(){
  adcStart(CP_READ);
}

void ControlPilot::BeginPulse(){
  //Start the timer and begin pulsing the CP line
  ledcAttachPin(CP_PWM, 0);
  
  ledcWrite(0, Configuration::GetCpPwmDutyCycle());

  //delayMicroseconds(30);

  intTimer = timerBegin(0, 80, true);

  //Timer to read CP voltage every 10ms
  timerAttachInterrupt(intTimer, &read_cp, true);
  timerAlarmWrite(intTimer, 10000, true);
  timerAlarmEnable(intTimer);

  pulsing = true;
}

void ControlPilot::EndPulse(){
  timerEnd(intTimer);
  ledcDetachPin(CP_PWM);
  digitalWrite(CP_PWM, HIGH);

  pulsing = false;
}

CpState ControlPilot::State(){
    int cpValue;

    if(pulsing){
        cpValue = adcEnd(CP_READ);
    }
    else{
        cpValue = analogRead(CP_READ);
    }
    

    Serial.println(cpValue);

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
