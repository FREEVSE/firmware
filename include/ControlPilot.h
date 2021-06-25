#pragma once

/**
 * @brief Possible states of the control pilot
 * @details 
 * - Idle, VehicleDetected, Charge and ChargeWithVentilation correspond to the J1772 states
 * - Invalid represents as state that does not fall within the values of one of the J1772 states
 */
enum class CpState{
    Idle,
    VehicleDetected,
    Charge,
    ChargeWithVentilation,
    Invalid
};

/**
 * @brief Actions performed in the pulse ISR
 * @details
 * - PulseHigh Transitions the CP line to the HIGH state
 * - SampleHigh Samples the CP line and records the value in lastHighValue
 * - PulseLow Return the CP line to the LOW state
 * - SampleLow Samples the CP line and recors the value in lastLowValue
 */
enum Action{
    PulseHigh,
    SampleHigh,
    PulseLow,
    SampleLow
};

class ControlPilot{
    public:
        /**
         * @brief Initializes the control pilot functionality
         */
        static void Init();

        /**
         * @brief Begins pulsing the CP line
         * @details Begins Control Pilot line pulsing by (re)setting a timer linked to an ISR
         * 
         * @return ESP_OK If pulsing started
         * @return ESP_FAIL If pulsing failed to start
         */
        static esp_err_t BeginPulse();

        /**
         * @brief End pulsing on the CP line
         * @details If the CP line was not pulsing, nothing happens
         */
        static void EndPulse();

        /**
         * @brief Gets the current state of the Control Pilot line
         * @details This is the value of the CP line since the last cycle was completed.
         * 
         * @return CpState 
         */
        static CpState State();

        /**
         * @brief Gets the raw high and low ADC values of the last CP line read
         * 
         * @return std::tuple<int, int> 1. Last High value, 2. Last low value
         */
        static std::tuple<int, int> Raw();

        /**
         * @brief Returns the string representation of the CP state
         * 
         * @param value A CP state to stringify
         * @return const char* Pointer to the string
         */
        static const char* ToString(const CpState value);

    private:
        static void IRAM_ATTR Pulse(void *);
        static Action nextAction;        
        static bool pulsing;
        static volatile unsigned int lastHighValue;
        static volatile int lastLowValue;
        static volatile CpState lastState;
        static int highTime;
};