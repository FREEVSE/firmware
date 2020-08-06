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

        static void IRAM_ATTR PulseHigh(void *);
        static void IRAM_ATTR PulseLow(void *);
        static void IRAM_ATTR Sample(void *);        
        static bool pulsing;
        static int lastCpValue;
        static int highTime;
};