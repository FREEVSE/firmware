#include <Configuration.h>
#include <ControlPilot.h>
#include <Arduino.h>

#include <driver/timer.h>
#include <driver/gpio.h>
#include <driver/adc.h>
#include <soc/soc.h>

#define TAG "CP"

#define DELAY_PULSEHIGH_SAMPLEHIGH 30
#define DELAY_PULSELOW_SAMPLELOW 30

//TODO Fix all the hardcoded timer groups and numbers

Action ControlPilot::nextAction;
bool ControlPilot::pulsing = false;
int ControlPilot::highTime = 0;

volatile unsigned int ControlPilot::lastHighValue = 0;
volatile int ControlPilot::lastLowValue = -1;
volatile CpState ControlPilot::lastState = CpState::Idle;

/**
 * ControlPilot::Init.
 * 
 * Initializes the Control Pilot module (timers, interrups, etc...)
 *
 * @author	Matthew Goulart
 * @since	v0.0.1
 * @version	v1.0.0	Tuesday, November 17th, 2020.
 * @global
 * @return	void
 */
void ControlPilot::Init(){
    #ifdef EN_CP
    pinMode(CP_PWM_PIN, OUTPUT);

    //Set analog read res to a lower value for slightly more linear curve.
    analogReadResolution(9);

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
    #endif
}


/**
 * ControlPilot::Pulse.
 * 
 * Interrupt handler to pulse the Control Pilot line at specific intervals
 *
 * @author	Matthew Goulart
 * @since	v0.0.1
 * @version	v1.0.0	Tuesday, November 17th, 2020.
 * @global
 * @param	void*	Any arguments received	
 * @return	void
 */
void IRAM_ATTR ControlPilot::Pulse(void* arg){
    TIMERG0.int_clr_timers.t0 = 1;

    int nextActionDelay = 0;
    ulong startMicros = micros();
    ulong duration;

    switch(nextAction){
        //Pulse the CP line HIGH
        case Action::PulseHigh:
            digitalWrite(CP_PWM_PIN, LOW);
            nextActionDelay = DELAY_PULSEHIGH_SAMPLEHIGH;
            nextAction = Action::SampleHigh;
            break;
        
        //Sample the CP line
        case Action::SampleHigh:
            lastHighValue = adc1_get_raw(ADC1_CHANNEL_5); //analogRead(CP_READ_PIN);
            nextActionDelay = highTime - DELAY_PULSEHIGH_SAMPLEHIGH;
            nextAction = Action::PulseLow;
            break;

        //CP line back to LOW
        case Action::PulseLow:
            digitalWrite(CP_PWM_PIN, HIGH);
            nextActionDelay = DELAY_PULSELOW_SAMPLELOW;
            nextAction = Action::SampleLow;
            break;

        case Action::SampleLow:
            lastLowValue = adc1_get_raw(ADC1_CHANNEL_5);
            nextActionDelay = CP_PWM_PERIOD_US - highTime - DELAY_PULSELOW_SAMPLELOW;
            nextAction = Action::PulseHigh;
            break;

        //TODO: Sample CP line when low
    }

    duration = micros() - startMicros;

    TIMERG0.hw_timer[0].alarm_low = nextActionDelay - duration;    //Set next alarm. Since it's always less than 1000, we can set only the first 32 bits.
    TIMERG0.hw_timer[0].alarm_high = 0x0;               //... Just to be sure
    TIMERG0.hw_timer[0].config.alarm_en = 1;            //We need to re-enable the alarm
}

/**
 * ControlPilot::BeginPulse.
 * 
 * Begins Control Pilot line pulsing by (re) setting a timer linked to an ISR
 *
 * @author	Matthew Goulart
 * @since	v0.0.1
 * @version	v1.0.0	Tuesday, November 17th, 2020.
 * @global
 * @return	ESP_OK is pulsing started, ESP_FAIL if not
 */
esp_err_t ControlPilot::BeginPulse(){
    #ifdef EN_CP
    if(pulsing)
        return ESP_OK;

    nextAction = Action::PulseHigh;
    pulsing = true;

    ESP_ERROR_CHECK(timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 1000));
    ESP_ERROR_CHECK(timer_start(timer_group_t::TIMER_GROUP_0, timer_idx_t::TIMER_0));

    #endif
    return ESP_OK;
}

/**
 * ControlPilot::EndPulse.
 * 
 * End Control Pilot line pulsing by pausing the timer.
 *
 * @author	Matthew Goulart
 * @since	v0.0.1
 * @version	v1.0.0	Tuesday, November 17th, 2020.
 * @global
 * @return	void
 */
void ControlPilot::EndPulse(){
    #ifdef EN_CP
    if(!pulsing)
        return;

    ESP_ERROR_CHECK(timer_pause(TIMER_GROUP_0, TIMER_0));
    
    digitalWrite(CP_PWM_PIN, LOW);

    //Set an invalid value so that we don't accidentally read a valid value
    //next time we start pulsing and checking the CP state
    lastLowValue = -1;

    pulsing = false;
    #endif
}


/**
 * ControlPilot::State.
 * 
 * Computes the current state of the Control Pilot line.
 * This will block if the CP hasn't had a full cycle to read
 * both low and high voltages right after starting to pulse.
 *
 * @author	Matthew Goulart
 * @since	v0.0.1
 * @version	v1.0.0	Tuesday, November 17th, 2020.
 * @global
 * @return	Computed state of Control Pilot Line
 */
CpState ControlPilot::State(){
    #ifdef EN_CP
    int highVal = 0;

    if (pulsing)
    {
        //If we haven't had time to read the first pulse, return the last state (should be idle)
        if(lastLowValue < 0){ 
            ESP_LOGI(TAG, "Still waiting for first pulse");
            return lastState; 
            }

        //THIS LINE IS EXTREMELY IMPORTANT
        //If the low value isn't roughly -12V, then either the vehicle's diode has failed
        //or something else is touching the CP and PE pins (like a curious child's fingers)
        if (lastLowValue >= CP_LOW_MAX) { 
            ESP_LOGI(TAG, "Low value (%i) was above 0 threshold (%i)", lastLowValue, CP_LOW_MAX);
            return lastState = CpState::Invalid; 
            }

        highVal = lastHighValue;
    }
    else
    {
        highVal = analogRead(CP_READ_PIN);
    }

    CpState state = CpState::Invalid;

    if (highVal >= CP_IDLE_MIN)
    {
        state = CpState::Idle;
    }
    else if (highVal <= CP_PRESENT_MAX && highVal >= CP_PRESENT_MIN)
    {
        state = CpState::VehicleDetected;
    }
    else if (highVal <= CP_CHARGING_MAX && highVal >= CP_CHARGING_MIN)
    {
        state = CpState::Charge;
    }
    else if (highVal <= CP_WITH_VENT_MAX && highVal >= CP_WITH_VENT_MIN)
    {
        state = CpState::ChargeWithVentilation;
    }

    return lastState = state;
    #else
    return CpState::Idle;
    #endif
}

std::tuple<int, int> ControlPilot::Raw(){
    return std::make_tuple(lastHighValue, lastLowValue);
}

/**
 * ControlPilot::ToString.
 * 
 * Gets the Control Pilot state in plain-text
 *
 * @author	Matthew Goulart
 * @since	v0.0.1
 * @version	v1.0.0	Tuesday, November 17th, 2020.
 * @access	const
 * @global
 * @param	cpstate	The CP State to convert to text	
 * @return	C_STR representing the current CP state
 */
const char* ControlPilot::ToString(const CpState value)
{
    static const char* LUT[] = {"Idle", "VehicleDetected", "Charge", "ChargeWithVentilation", "Invalid" };
    return LUT[static_cast<int>(value)];
}
