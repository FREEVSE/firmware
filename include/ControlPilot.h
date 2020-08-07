#pragma once

#include <Arduino.h>

enum class CpState{
    Idle,
    VehicleDetected,
    Charge,
    ChargeWithVentilation,
    Invalid
};

class ControlPilot{
    public:
        static void Init();

        static void BeginPulse();
        static void EndPulse();

        static CpState State();
        static const char* ToString(const CpState value);

    private:
        static hw_timer_t * intTimer;   

        static void IRAM_ATTR PulseHigh();
        static void IRAM_ATTR PulseLow();
        static void IRAM_ATTR Sample();        
        static bool pulsing;
        static int lastCpValue;
        static int highTime;
};