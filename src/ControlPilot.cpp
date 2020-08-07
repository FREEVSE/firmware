#include <Configuration.h>
#include <ControlPilot.h>
#include <Arduino.h>

#include <driver/timer.h>
#include <driver/gpio.h>
#include <soc/soc.h>

Action ControlPilot::nextAction;
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
    timer_config_t tConfig = {
        .alarm_en = true,
        .counter_en = false,
        .intr_type = TIMER_INTR_LEVEL,
        .counter_dir = TIMER_COUNT_UP,
        .auto_reload = true,
        .divider = 80, //Prescaler of 80 gives us a resolution of 1uS
    };
    ESP_ERROR_CHECK(timer_init(TIMER_GROUP_0, TIMER_0, &tConfig));
    ESP_ERROR_CHECK(timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0)); //Timer will reset to this value on alarm
}

void IRAM_ATTR ControlPilot::Pulse(void* arg){
    TIMERG0.int_clr_timers.t0 = 1;
    TIMERG0.hw_timer[0].config.alarm_en = 1;

    short nextActionDelay = 0;

    switch(nextAction){
        case Action::PulseHigh:
            digitalWrite(CP_PWM_PIN, LOW);
            nextActionDelay = 30;
            nextAction = Action::Sample;
            break;
        
        case Action::Sample:
            digitalWrite(32, HIGH);
            adcStart(CP_READ_PIN);
            digitalWrite(32, LOW);
            nextActionDelay = highTime - 30;
            nextAction = Action::PulseLow;
            break;

        case Action::PulseLow:
            digitalWrite(CP_PWM_PIN, HIGH);
            nextActionDelay = CP_PWM_FREQ - highTime;
            nextAction = Action::PulseHigh;
            break;
    }

    TIMERG0.hw_timer[0].alarm_low = nextActionDelay;
    TIMERG0.hw_timer[0].alarm_high = 0x0;
}

void ControlPilot::BeginPulse(){
    if(pulsing)
        return;

    nextAction = Action::PulseHigh;

    ESP_ERROR_CHECK(timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 1000));
    ESP_ERROR_CHECK(timer_enable_intr(TIMER_GROUP_0, TIMER_0));
    ESP_ERROR_CHECK(timer_isr_register(timer_group_t::TIMER_GROUP_0, timer_idx_t::TIMER_0, Pulse, NULL, 0, NULL));
    ESP_ERROR_CHECK(timer_start(timer_group_t::TIMER_GROUP_0, timer_idx_t::TIMER_0));
    
    pulsing = true;
}

void ControlPilot::EndPulse(){
    if(!pulsing)
        return;

    ESP_ERROR_CHECK(timer_pause(TIMER_GROUP_0, TIMER_0));
    ESP_ERROR_CHECK(timer_disable_intr(TIMER_GROUP_0, TIMER_0));
    
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
