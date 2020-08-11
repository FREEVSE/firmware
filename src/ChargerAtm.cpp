#include <ChargerAtm.h>
#include <Automaton.h>
#include <Configuration.h>

#include <ControlPilot.h>
#include <GFCI.h>
#include <ACDetector.h>

#define HANDLE_ERROR(msg)\
    lastError = msg;\
    trigger(EVT_ERROR);\
    return;

ChargerAtm & ChargerAtm::begin(){
    const static state_t state_table[] PROGMEM = {
        /*                          ON_ENTER        ON_LOOP         ON_EXIT     EVT_VEHICLE_DISCONNECTED     EVT_VEHICLE_DETECTED    EVT_CHARGE_REQUESTED   EVT_NO_EARTH   EVT_GFCI_TRIP   EVT_ERROR     ELSE */
        /* POST */                  ENT_POST,            -1,             -1,                          -1,                      -1,                     -1,            -1,             -1,      ERROR,      -1,
        /* IDLE */                  ENT_IDLE,            -1,             -1,                          -1,        VEHICLE_DETECTED,                     -1,            -1,          FAULT,      ERROR,      -1,
        /* VEHICLE_DETECTED */        ENT_VD,       LOOP_VD,         EXT_VD,                        IDLE,                      -1,               CHARGING,            -1,          FAULT,      ERROR,      -1,
        /* CHARGING */          ENT_CHARGING,            -1,   EXT_CHARGING,                        IDLE,        VEHICLE_DETECTED,                     -1,         ERROR,          FAULT,      ERROR,      -1,
        /* FAULT */                       -1,            -1,             -1,                          -1,                      -1,                     -1,            -1,             -1,      ERROR,      -1,
        /* ERROR */                ENT_ERROR,            -1,             -1,                          -1,                      -1,                     -1,            -1,             -1,         -1,      -1
    };

    Machine::begin(state_table, ELSE);

    return *this;
}

void ChargerAtm::cycle(){
    cpState = ControlPilot::State(); //Cache CP state so we dont try to read it for each event check
    Machine::cycle();
}

void ChargerAtm::EnterPOST(){
    //GFCI test
    Serial.println("Starting GFCI self test");
    if(!GFCI::SelfTest()) { HANDLE_ERROR("GFCI self test failed") }
        
    //Close CTR to test GND and determine L1/2
    digitalWrite(CTR_ENER_PIN, HIGH);
    delay(500);

    if(!ACDetector::IsL1Present()) { HANDLE_ERROR("No earth") }
    if(ACDetector::IsL2Present()) { level = Level::L2; }

    digitalWrite(CTR_ENER_PIN, LOW);

    state(IDLE);
}

void ChargerAtm::EnterIdle(){
    ControlPilot::EndPulse();
}

void ChargerAtm::EnterVehicleDetected(){
    ControlPilot::BeginPulse();
}

void ChargerAtm::EnterCharging(){
    digitalWrite(CTR_ENER_PIN, HIGH);
    delay(250); //Give the contactor some time to close
}

void ChargerAtm::ExitCharging(){
    digitalWrite(CTR_ENER_PIN, LOW);
}

void ChargerAtm::EnterError(){
    Serial.println(lastError);
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

        case EVT_NO_EARTH:
            return !ACDetector::IsL1Present();

        case EVT_GFCI_TRIP:
            return !GFCI::State();

        case EVT_ERROR:
            return false;

        return 0;
    }

    return 0;
}

void ChargerAtm::action(int id){
    switch(id){
        case ENT_POST: EnterPOST(); break;
        case ENT_IDLE: EnterIdle(); break;
        case ENT_VD: EnterVehicleDetected(); break;
        case ENT_CHARGING: EnterCharging(); break;
        case EXT_CHARGING: ExitCharging(); break;
        case ENT_ERROR: EnterError(); break;
    }
}

ChargerAtm & ChargerAtm::trace( Stream & stream ) {
  Machine::setTrace( &stream, atm_serial_debug::trace,
    "CHARGER\0EVT_VEHICLE_DISCONNECTED\0EVT_VEHICLE_DETECTED\0EVT_CHARGE_REQUESTED\0EVT_NO_EARTH\0EVT_GFCI_TRIP\0EVT_ERROR\0ELSE\0POST\0IDLE\0VEHICLE_DETECTED\0CHARGING\0FAULT\0ERROR");
  return *this;
}