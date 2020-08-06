#include <Configuration.h>
#include <ControlPilot.h>
#include <Arduino.h>

#include <driver/timer.h>

#define ISR_DELAY 5

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

    //Set up our timer. We use IDF here because arduino makes too many assumptions
    timer_config_t tConfig;
    tConfig.divider = 80; //Prescaler of 80 gives us a resolution of 1uS
    tConfig.counter_dir = timer_count_dir_t::TIMER_COUNT_UP;
    tConfig.counter_en = timer_start_t::TIMER_PAUSE;
    tConfig.alarm_en = true;
    tConfig.auto_reload = true;

    timer_init(timer_group_t::TIMER_GROUP_0, timer_idx_t::TIMER_0, &tConfig);
    timer_set_counter_value(timer_group_t::TIMER_GROUP_0, timer_idx_t::TIMER_0, 0x00000000ULL); //Timer will reset to this value on alarm
    
}

void IRAM_ATTR ControlPilot::PulseLow(void *){
    Serial.println("pulse low");
    digitalWrite(CP_PWM_PIN, HIGH);

    timer_set_alarm_value(timer_group_t::TIMER_GROUP_0, timer_idx_t::TIMER_0, CP_PWM_FREQ - highTime);
    timer_isr_register(timer_group_t::TIMER_GROUP_0, timer_idx_t::TIMER_0, PulseHigh, NULL, ESP_INTR_FLAG_IRAM, NULL);
    timer_enable_intr(timer_group_t::TIMER_GROUP_0, timer_idx_t::TIMER_0);    
}

void IRAM_ATTR ControlPilot::PulseHigh(void *){
    Serial.println("pulse high");
    digitalWrite(CP_PWM_PIN, LOW);

    timer_set_alarm_value(timer_group_t::TIMER_GROUP_0, timer_idx_t::TIMER_0, 30);
    timer_isr_register(timer_group_t::TIMER_GROUP_0, timer_idx_t::TIMER_0, Sample, NULL, ESP_INTR_FLAG_IRAM, NULL);
    timer_enable_intr(timer_group_t::TIMER_GROUP_0, timer_idx_t::TIMER_0);
}

void IRAM_ATTR ControlPilot::Sample(void *){
    Serial.println("sample");
    digitalWrite(32, HIGH);
    adcStart(CP_READ_PIN);
    digitalWrite(32, LOW);

    timer_set_alarm_value(timer_group_t::TIMER_GROUP_0, timer_idx_t::TIMER_0, highTime - 30);
    timer_isr_register(timer_group_t::TIMER_GROUP_0, timer_idx_t::TIMER_0, PulseLow, NULL, ESP_INTR_FLAG_IRAM, NULL);
    timer_enable_intr(timer_group_t::TIMER_GROUP_0, timer_idx_t::TIMER_0);
}

void ControlPilot::BeginPulse(){
    Serial.println("Begin pulse...");
    if(pulsing)
        return;

    timer_set_alarm_value(timer_group_t::TIMER_GROUP_0, timer_idx_t::TIMER_0, 100);
    timer_enable_intr(timer_group_t::TIMER_GROUP_0, timer_idx_t::TIMER_0);
    timer_isr_register(timer_group_t::TIMER_GROUP_0, timer_idx_t::TIMER_0, PulseHigh, NULL, ESP_INTR_FLAG_IRAM, NULL);    
    timer_start(timer_group_t::TIMER_GROUP_0, timer_idx_t::TIMER_0);

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
