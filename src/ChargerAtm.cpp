#include <ChargerAtm.h>
#include <Automaton.h>
#include <Configuration.h>

#include <ControlPilot.h>
#include <GFCI.h>


ChargerAtm & ChargerAtm::begin(){
    const static state_t state_table[] PROGMEM = {
        /*                          ON_ENTER    ON_LOOP         ON_EXIT     EVT_VEHICLE_DISCONNECTED     EVT_VEHICLE_DETECTED    EVT_CHARGE_REQUESTED   EVT_GFCI_TRIP     ELSE */
        /* IDLE */                  ENT_IDLE,        -1,             -1,                          -1,        VEHICLE_DETECTED,                     -1,          FAULT,      -1,
        /* VEHICLE_DETECTED */        ENT_VD,   LOOP_VD,         EXT_VD,                        IDLE,                      -1,               CHARGING,          FAULT,      -1,
        /* CHARGING */          ENT_CHARGING,        -1,   EXT_CHARGING,                        IDLE,        VEHICLE_DETECTED,                     -1,          FAULT,      -1, 
        /* FAULT */                       -1,        -1,             -1,                          -1,                      -1,                     -1,             -1,      -1,
        /* ERROR */                       -1,        -1,             -1,                          -1,                      -1,                     -1,             -1,      -1
    };

    Machine::begin(state_table, ELSE);

    return *this;
}

void ChargerAtm::cycle(){
    cpState = ControlPilot::State(); //Cache CP state so we dont try to read it for each event check
    Machine::cycle();
}

void ChargerAtm::EnterIdle(){
    ControlPilot::EndPulse();
}

void ChargerAtm::EnterVehicleDetected(){
    ControlPilot::BeginPulse();
}

void ChargerAtm::EnterCharging(){
    //GFCI::Inhibit();
    digitalWrite(CTR_ENER_PIN, HIGH);
}

void ChargerAtm::ExitCharging(){
    digitalWrite(CTR_ENER_PIN, LOW);
}

int ChargerAtm::event(int id){
    //CpState cpState = ControlPilot::State();

    switch(id){
        case EVT_VEHICLE_DISCONNECTED:
            return cpState == CpState::Idle;

        case EVT_VEHICLE_DETECTED:
            return cpState == CpState::VehicleDetected;
            
        case EVT_CHARGE_REQUESTED:
            return cpState == CpState::Charge || cpState == CpState::ChargeWithVentilation;

        case EVT_GFCI_TRIP:
            return !GFCI::State();

        return 0;
    }
}

void ChargerAtm::action(int id){
    switch(id){
        case ENT_IDLE: EnterIdle(); break;
        case ENT_VD: EnterVehicleDetected(); break;
        case ENT_CHARGING: EnterCharging(); break;
        case EXT_CHARGING: ExitCharging(); break;
    }
}

ChargerAtm & ChargerAtm::trace( Stream & stream ) {
  Machine::setTrace( &stream, atm_serial_debug::trace,
    "CHARGER\0EVT_VEHICLE_DISCONNECTED\0EVT_VEHICLE_DETECTED\0EVT_CHARGE_REQUESTED\0EVT_GFCI_TRIP\0ELSE\0IDLE\0VEHICLE_DETECTED\0CHARGING\0FAULT\0ERROR");
  return *this;
}