#include <ChargerAtm.h>
#include <Automaton.h>

#include <ControlPilot.h>
#include <GFCI.h>

ChargerAtm & ChargerAtm::begin(){
    const static state_t state_table[] PROGMEM = {
        /*                      ON_ENTER    ON_LOOP     ON_EXIT     EVT_VEHICLE_DISCONNECTED     EVT_VEHICLE_DETECTED    EVT_CHARGE_REQUESTED   EVT_GFCI_TRIP     ELSE */
        /* IDLE */              ENT_IDLE,        -1,         -1,                          -1,        VEHICLE_DETECTED,                     -1,          ERROR,      -1,
        /* VEHICLE_DETECTED */    ENT_VD,   LOOP_VD,     EXT_VD,                        IDLE,                      -1,               CHARGING,          ERROR,      -1,
        /* CHARGING */                -1,        -1,         -1,                        IDLE,        VEHICLE_DETECTED,                     -1,          ERROR,      -1, 
        /* FAULT */                   -1,        -1,         -1,                          -1,                      -1,                     -1,          ERROR,      -1,
        /* ERROR */                   -1,        -1,         -1,                          -1,                      -1,                     -1,          ERROR,      -1
    };

    Machine::begin(state_table, ELSE);

    return *this;
}

void ChargerAtm::EnterIdle(){
    Serial.println("Entering IDLE state");
    ControlPilot::EndPulse();
}

void ChargerAtm::EnterVehicleDetected(){
    Serial.println("Entering VEHICLE_DETECTED state");
    ControlPilot::BeginPulse();
}

int ChargerAtm::event(int id){
    CpState cpState = ControlPilot::State();

    switch(id){
        case EVT_VEHICLE_DISCONNECTED:
            return cpState == CpState::Idle;

        case EVT_VEHICLE_DETECTED:
            return cpState == CpState::VehicleDetected;
            
        case EVT_CHARGE_REQUESTED:
            return cpState == CpState::Charge || cpState == CpState::ChargeWithVentilation;

        case EVT_GFCI_TRIP:
            return 0;

        return 0;
    }
}

void ChargerAtm::action(int id){
    switch(id){
        case ENT_IDLE: EnterIdle(); break;
        case ENT_VD: EnterVehicleDetected(); break;
    }
}

ChargerAtm & ChargerAtm::trace( Stream & stream ) {
  Machine::setTrace( &stream, atm_serial_debug::trace,
    "CHARGER\0EVT_VEHICLE_DISCONNECTED\0EVT_VEHICLE_DETECTED\0EVT_CHARGE_REQUESTED\0EVT_GFCI_TRIP\0ELSE\0IDLE\0VEHICLE_DETECTED\0CHARGING\0FAULT\0ERROR");
  return *this;
}