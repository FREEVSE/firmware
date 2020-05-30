#include <ChargerAtm.h>
#include <Automaton.h>

ChargerAtm::ChargerAtm(){

}

ChargerAtm & ChargerAtm::begin(){
    const static state_t state_table[] PROGMEM = {
        /*                      ON_ENTER    ON_LOOP     ON_EXIT     ELSE */
        /* IDLE */                    -1,       -1,         -1,       -1,
        /* VEHICLE_DETECTED */        -1,       -1,         -1,       -1,
        /* CHARGING */                -1,       -1,         -1,       -1,      
        /* CHARGIN_WITH_VENT */       -1,       -1,         -1,       -1,
    };

    Machine::begin(state_table, ELSE);

    return *this;

}