#pragma once

#include <Automaton.h>
#include <ControlPilot.h>

class ChargerAtm: public Machine{
    public:
        ChargerAtm(void) : Machine(){};

        /**
         * @brief States
         * @details These are the possible states that the state machine can be in.
         */
        enum states{ 
            POST, 
            IDLE, 
            VEHICLE_DETECTED, 
            CHARGING, 
            UPDATING, 
            FAULT, 
            ERROR};

        /**
         * @brief Events
         * @details These are events that can occur. Events can trigger actions depending on the current state.
         */
        enum events{ 
            EVT_VEHICLE_DISCONNECTED, 
            EVT_VEHICLE_DETECTED, 
            EVT_CHARGE_REQUESTED, 
            EVT_CP_INVALID, 
            EVT_UPDATE, 
            EVT_NO_EARTH, 
            EVT_GFCI_TRIP, 
            EVT_ERROR, 
            ELSE };
        
        /**
         * @brief Actions
         * @details Actions occur when an event triggers them. A state can perform specific actions when an evvent occurs.
         */
        enum actions{ 
            ENT_POST,
            ENT_IDLE, LOOP_IDLE, EXT_IDLE,
            ENT_VD, EXT_VD,
            ENT_CHARGING, EXT_CHARGING,
            ENT_UPDATING,
            ENT_FAULT,
            ENT_ERROR
        };

        /**
         * @brief The two possible charger levels
         * @details As per J1772, L1 is a 120v evse and L2 is 240v
         * 
         */
        enum Level { L1, L2 };

        /**
         * @brief Prepares the state machine to be run
         * @details Sets up the state table and prepares the machine to run
         * @return ChargerAtm& The ChargerAtm instance
         */
        ChargerAtm & begin();

        /**
         * @brief Checks if an event occured
         * 
         * @param id The ID of the event to check for
         * @return 1 if event occured
         * @return 0 if event did not occur
         */
        int event(int id);

        /**
         * @brief Performs an action based on the ID
         * 
         * @param id The ID of the actionb to perform
         */
        void action(int id);

        /**
         * @brief Runs the state machine one "step"
         * 
         */
        void cycle();

        /**
         * @brief Prints debug information to the given stream
         * 
         * @param stream The stream to print to
         * @return ChargerAtm& The ChargerAtm instance
         */
        ChargerAtm & trace(Stream & stream);

    private:
        void EnterPOST();
        void EnterIdle();
        void EnterVehicleDetected();
        void EnterCharging();
        void LoopCharging();
        void ExitCharging();
        void EnterUpdating();
        void EnterError();

        char *lastError;
        Level level;

        CpState cpState;
};