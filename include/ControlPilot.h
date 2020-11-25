#pragma once

enum class CpState{
    Idle,
    VehicleDetected,
    Charge,
    ChargeWithVentilation,
    Invalid,
    Wait
};

enum Action{
    PulseHigh,
    SampleHigh,
    PulseLow,
    SampleLow
};

class ControlPilot{
    public:
        static void Init();

        static esp_err_t BeginPulse();
        static void EndPulse();

        static CpState State();
        static const char* ToString(const CpState value);

    private:
        static void IRAM_ATTR Pulse(void *);
        static Action nextAction;        
        static bool pulsing;
        static volatile unsigned int lastCpValue;
        static volatile int lastLowValue;
        static volatile CpState lastState;
        static int highTime;
};