#pragma once

enum class State{
    Idle,
    VehicleDetected,
    Charging,
    ChargingWithVentilation,
    Fault,
    Error
};

class ChargerStateMachine{
    public:
        static void Init();
        static void Run();

        static State state;
    private:
        static void IdleState();
        static bool IdleToVehicleDetectedTransition();
        static bool IdleToFaultTransition();
        static bool IdleToErrorTransition();

        static void VehicleDetectedState();
        static bool VehicleDetectedToIdleTransition();
        static bool VehicleDetectedToChargingTransition();

        static void ChargingState();
        static void ChargingWithVentilationState();

        static void FaultState();
        static void ErrorState();

};