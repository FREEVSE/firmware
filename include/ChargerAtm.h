#pragma once

#include <Automaton.h>
#include <ControlPilot.h>

class ChargerAtm: public Machine{
    public:
        ChargerAtm(void) : Machine(){};

        enum { IDLE, VEHICLE_DETECTED, CHARGING, FAULT, ERROR}; //States
        enum { EVT_VEHICLE_DISCONNECTED, EVT_VEHICLE_DETECTED, EVT_CHARGE_REQUESTED, EVT_GFCI_TRIP, ELSE }; //Events
        enum { 
            ENT_IDLE, LOOP_IDLE, EXT_IDLE,
            ENT_VD, LOOP_VD, EXT_VD,
            ENT_CHARGING, LOOP_CHARGING, EXT_CHARGING,
            ENT_FAULT,
            ENT_ERROR
        };//Actions

        ChargerAtm & begin();
        int event(int id);
        void action(int id);
        void cycle();
        ChargerAtm & trace(Stream & stream);

    private:
        void EnterIdle();
        void EnterVehicleDetected();
        void EnterCharging();
        void ExitCharging();

        CpState cpState;
};