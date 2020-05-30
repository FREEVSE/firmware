#pragma once

#include <Automaton.h>

class ChargerAtm: public Machine{
    public:
        ChargerAtm(void) : Machine(){};

        enum { IDLE, VEHICLE_DETECTED, CHARGING, CHARGING_WITH_VENT, FAULT, ERROR}; //States
        enum { ELSE }; //Events

        ChargerAtm & begin();

};