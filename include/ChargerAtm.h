#pragma once

#include <Automaton.h>
#include <ControlPilot.h>

class ChargerAtm: public Machine{
    public:
        ChargerAtm(void) : Machine(){};

        enum { POST, IDLE, VEHICLE_DETECTED, CHARGING, FAULT, ERROR}; //States
        enum { EVT_VEHICLE_DISCONNECTED, EVT_VEHICLE_DETECTED, EVT_CHARGE_REQUESTED, EVT_NO_EARTH, EVT_GFCI_TRIP, EVT_ERROR, ELSE }; //Events
        enum { 
            ENT_POST,
            ENT_IDLE, LOOP_IDLE, EXT_IDLE,
            ENT_VD, LOOP_VD, EXT_VD,
            ENT_CHARGING, EXT_CHARGING,
            ENT_FAULT,
            ENT_ERROR
        };//Actions

        enum Level { L1, L2 };

        ChargerAtm & begin();
        int event(int id);
        void action(int id);
        void cycle();
        ChargerAtm & trace(Stream & stream);

    private:
        void EnterPOST();
        void EnterIdle();
        void EnterVehicleDetected();
        void EnterCharging();
        void LoopCharging();
        void ExitCharging();
        void EnterError();

        char *lastError;
        Level level;

        CpState cpState;
};