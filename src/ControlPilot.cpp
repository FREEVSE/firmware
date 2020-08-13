#include <Configuration.h>
#include <ControlPilot.h>
#include <Arduino.h>

#include <driver/timer.h>
#include <driver/gpio.h>
#include <driver/adc.h>
#include <soc/soc.h>

//TODO Fix all the hardcoded timer groups and numbers

Action ControlPilot::nextAction;
bool ControlPilot::pulsing = false;
int ControlPilot::highTime = 0;

volatile int ControlPilot::lastCpValue = 0;

void ControlPilot::Init(){
    pinMode(CP_PWM_PIN, OUTPUT);

    //adcAttachPin(CP_READ_PIN);
    adc1_config_width(ADC_WIDTH_9Bit);
    adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_0);

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
    ESP_ERROR_CHECK(timer_enable_intr(TIMER_GROUP_0, TIMER_0));
    ESP_ERROR_CHECK(timer_isr_register(timer_group_t::TIMER_GROUP_0, timer_idx_t::TIMER_0, Pulse, NULL, ESP_INTR_FLAG_IRAM ,NULL));
}


void IRAM_ATTR ControlPilot::Pulse(void* arg){
    TIMERG0.int_clr_timers.t0 = 1;

    int nextActionDelay = 0;

    switch(nextAction){
        //Pulse the CP line HIGH
        case Action::PulseHigh:
            digitalWrite(CP_PWM_PIN, LOW);
            nextActionDelay = 30;
            nextAction = Action::Sample;
            break;
        
        //Sample the CP line
        case Action::Sample:
            lastCpValue = adc1_get_raw(ADC1_CHANNEL_5); //analogRead(CP_READ_PIN);
            nextActionDelay = highTime - 30;
            nextAction = Action::PulseLow;
            break;

        //CP line back to LOW
        case Action::PulseLow:
            digitalWrite(CP_PWM_PIN, HIGH);
            nextActionDelay = CP_PWM_FREQ - highTime;
            nextAction = Action::PulseHigh;
            break;
    }

    TIMERG0.hw_timer[0].alarm_low = nextActionDelay;    //Set next alarm. Since it's always less than 1000, we can set only the first 32 bits.
    TIMERG0.hw_timer[0].alarm_high = 0x0;               //Just to be sure
    TIMERG0.hw_timer[0].config.alarm_en = 1;            //We need to re-enable the alarm
}

void ControlPilot::BeginPulse(){
    if(pulsing)
        return;

    nextAction = Action::PulseHigh;

    ESP_ERROR_CHECK(timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 1000));
    ESP_ERROR_CHECK(timer_start(timer_group_t::TIMER_GROUP_0, timer_idx_t::TIMER_0));
    
    pulsing = true;
}

void ControlPilot::EndPulse(){
    if(!pulsing)
        return;

    ESP_ERROR_CHECK(timer_pause(TIMER_GROUP_0, TIMER_0));
    
    digitalWrite(CP_PWM_PIN, LOW);

    pulsing = false;
}

CpState ControlPilot::State(){
    int cpValue;

    if(pulsing)
        cpValue = ControlPilot::lastCpValue;
    else
        cpValue = analogRead(CP_READ_PIN);
    
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
