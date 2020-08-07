#pragma once

#include <Arduino.h>

enum class CpState{
    Idle,
    VehicleDetected,
    Charge,
    ChargeWithVentilation,
    Invalid
};

enum Action{
    PulseHigh,
    Sample,
    PulseLow
};

class ControlPilot{
    public:
        static void Init();

        static void BeginPulse();
        static void EndPulse();

        static CpState State();
        static const char* ToString(const CpState value);

    private:
        static void IRAM_ATTR Pulse(void *);
        static Action nextAction;        
        static bool pulsing;
        static int lastCpValue;
        static int highTime;
};