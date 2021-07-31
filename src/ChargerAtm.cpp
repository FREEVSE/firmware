#include <atomic>
#include <esp_log.h>

#include <ChargerAtm.h>
//#include <Automaton.h>
#include <Configuration.h>

//#include <ControlPilot.h>
#include <Contactor.h>
#include <GFCI.h>
#include <ACDetector.h>
#include <LCD.h>
#include <WiFiManager.h>

#define HANDLE_ERROR(msg)\
    lastError = msg;\
    trigger(EVT_ERROR);\

ChargerAtm & ChargerAtm::begin(){
    const static state_t state_table[] PROGMEM = {
/*                          ON_ENTER    ON_LOOP         ON_EXIT     EVT_VEHICLE_DISCONNECTED     EVT_VEHICLE_DETECTED    EVT_CHARGE_REQUESTED     EVT_CP_INVALID   EVT_UPDATE   EVT_NO_EARTH   EVT_GFCI_TRIP   EVT_ERROR     ELSE */
/* POST */                  ENT_POST,        -1,             -1,                          -1,                      -1,                     -1,                -1,          -1,            -1,             -1,      ERROR,      -1,
/* IDLE */                  ENT_IDLE,        -1,             -1,                          -1,        VEHICLE_DETECTED,                     -1,             ERROR,    UPDATING,            -1,          FAULT,      ERROR,      -1,
/* VEHICLE_DETECTED */        ENT_VD,        -1,         EXT_VD,                        IDLE,                      -1,               CHARGING,             ERROR,          -1,            -1,          FAULT,      ERROR,      -1,
/* CHARGING */          ENT_CHARGING,        -1,   EXT_CHARGING,                        IDLE,        VEHICLE_DETECTED,                     -1,             ERROR,          -1,         ERROR,          FAULT,      ERROR,      -1,
/* UPDATING */          ENT_UPDATING,        -1,             -1,                          -1,                      -1,                     -1,                -1,          -1,            -1,             -1,      ERROR,      -1,
/* FAULT */                       -1,        -1,             -1,                          -1,                      -1,                     -1,                -1,          -1,            -1,             -1,      ERROR,      -1,
/* ERROR */                ENT_ERROR,        -1,             -1,                        IDLE,                      -1,                     -1,                -1,          -1,            -1,             -1,         -1,      -1
    };

    Machine::begin(state_table, ELSE);

    return *this;
}

void ChargerAtm::cycle(){
    cpState = ControlPilot::State(); //Cache CP state so we dont try to read it for each event check
    Machine::cycle();
}

void ChargerAtm::EnterPOST(){
    #ifndef NO_POST
    LCD::PrintStatus("Self test...");

    //GFCI test
    Serial.println("Starting GFCI self test");
    if(!GFCI::SelfTest()) {
        HANDLE_ERROR("GFCI test failed"); 
        return;
    }
        
    //Close CTR to test GND and determine L1/2
    Contactor::Close();
    delay(500);

    if(!ACDetector::IsL1Present()) { 
        HANDLE_ERROR("No earth"); 
        Contactor::Open();
        return;
    }

    if(ACDetector::IsL2Present()) { level = Level::L2; }

    #endif

    Contactor::Open();
    
    LCD::PrintCapabilities(Configuration::GetMaxOutputAmps(), !(bool)(int)level);
    state(IDLE);
}

void ChargerAtm::EnterIdle(){
    LCD::PrintStatus("Idle");
    LCD::ClearTimer();
    ControlPilot::EndPulse();
}

void ChargerAtm::EnterVehicleDetected(){
    LCD::PrintStatus("Vehicle detected");
    ControlPilot::BeginPulse();
}

void ChargerAtm::EnterCharging(){
    LCD::PrintStatus("Charging");
    LCD::StartTimer();
    Contactor::Close();
    delay(250); //Give the contactor some time to close
}

void ChargerAtm::ExitCharging(){
    LCD::StopTimer();
    Contactor::Open();
}

void ChargerAtm::EnterUpdating(){
    auto res = WiFiManager::CheckUpdate();

    if(std::get<0>(res)){
        LCD::PrintStatus("Updating...");

        if(WiFiManager::DoUpdate(std::get<1>(res)) != ESP_OK){
            Configuration::SetFailedUpdateCount();
            LCD::PrintStatus("Update failed!");
        }
        else { LCD::PrintStatus("OK - restart in 10s"); }

        delay(10000);
        esp_restart();

        //We should never make it here...
        HANDLE_ERROR("MANUAL RESTART");
    }

    state(IDLE);
}

void ChargerAtm::EnterError(){
    ControlPilot::EndPulse();
    LCD::PrintStatus(lastError);
    Serial.println(lastError);
}

int ChargerAtm::event(int id){
    switch(id){
        case EVT_VEHICLE_DISCONNECTED:
            return cpState == CpState::Idle;

        case EVT_VEHICLE_DETECTED:
            return cpState == CpState::VehicleDetected;
            
        case EVT_CHARGE_REQUESTED:
            return cpState == CpState::Charge || cpState == CpState::ChargeWithVentilation;

        case EVT_CP_INVALID:
            if(cpState == CpState::Invalid){
                lastError = "CP Error";
                return true;
            }

            return false;

        case EVT_UPDATE:
            return WiFiManager::IsUpdateAvailable.load() &&                     //Check if an update was found previously
                    Configuration::GetAutoUpdate() &&                           //Make sure we should be updating automatically
                    Configuration::GetFailedUpdateCount() <= UPDATE_RETY_COUNT; //Make sure we haven't tried this version too many times

        case EVT_NO_EARTH:
            if(!ACDetector::IsL1Present()){
                lastError = "PE Error";
                return true;
            }

            return false;

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
        case ENT_UPDATING: EnterUpdating(); break;
        case ENT_ERROR: EnterError(); break;
    }
}

ChargerAtm & ChargerAtm::trace( Stream & stream ) {
  Machine::setTrace( &stream, atm_serial_debug::trace,
    "CHARGER\0EVT_VEHICLE_DISCONNECTED\0EVT_VEHICLE_DETECTED\0EVT_CHARGE_REQUESTED\0EVT_CP_INVALID\0EVT_UPDATE\0EVT_NO_EARTH\0EVT_GFCI_TRIP\0EVT_ERROR\0ELSE\0POST\0IDLE\0VEHICLE_DETECTED\0CHARGING\0UPDATING\0FAULT\0ERROR");
  return *this;
}