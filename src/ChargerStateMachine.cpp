#include <ChargerStateMachine.h>

#include <ControlPilot.h>

void ChargerStateMachine::Init(){
    State* idleState =              sm.addState(&IdleState);
    State* vehicleDetectedState =   sm.addState(&VehicleDetectedState);
    State* chargingState =          sm.addState(&ChargingState);
    State* chargingWithVentState =  sm.addState(&ChargingWithVentilationState);
    State* faultState =             sm.addState(&FaultState);
    State* errorState =             sm.addState(&ErrorState);

    idleState->addTransition(&IdleToVehicleDetectedTransition, vehicleDetectedState);
    idleState->addTransition(&IdleToFaultTransition, faultState);
    idleState->addTransition(&IdleToErrorTransition, errorState);
}

State ChargerStateMachine::state = State::Idle;

void ChargerStateMachine::Run(){
    switch (state)
    {
    case State::Idle:
        //Idle routine
        IdleState();

        //State transitions
        if(IdleToVehicleDetectedTransition()){
            state = State::VehicleDetected;
            break;
        }

        if(IdleToFaultTransition()){
            state = State::Fault;
            break;
        }

        if(IdleToErrorTransition()){
            state = State::Error;
            break;
        }
            
        break;
    
    default:
        break;
    }
}


// **Idle state and transitions**
//In this state, the charger waits for a vehicle to be detected
//and periodically tests itself
void ChargerStateMachine::IdleState(){

}

//Occurs when a vehicle is connected
bool ChargerStateMachine::IdleToVehicleDetectedTransition(){
    return ControlPilot::State() == CpState::VehicleDetected;
}

//Occurs if the Control Pilot is in a state other than Idle
bool ChargerStateMachine::IdleToFaultTransition(){
    CpState cpState = ControlPilot::State();

    if(cpState != CpState::Idle)
        return true;
}

//Occurs ...
bool ChargerStateMachine::IdleToErrorTransition(){

}

// **Vehicle Detected state and transitions**
//In this state, the charger begins advertising it's current capabilities
//by pulsing the Control Pilot with a specific duty cycle
void ChargerStateMachine::VehicleDetectedState(){
    if(sm.executeOnce){
        Serial.println("Beginning CP pulsing...");
        ControlPilot::BeginPulse();
    }
}

//Occurs if a vehicle that was connected but not charging was disconnected
bool ChargerStateMachine::VehicleDetectedToIdleTransition(){
    return ControlPilot::State() == CpState::Idle;
}

//Occurs when a connected vehicle is satisfied with the charger's capabilities
//and requests charging
bool ChargerStateMachine::VehicleDetectedToChargingTransition(){
    return ControlPilot::State() == CpState::Charge;
}

// **Charging state and transition**
//In this state, the charger resets the GFCI, energizes the contactor and
//continuously monitors mains voltage presence, GFCI and ambient temperature
void ChargerStateMachine::ChargingState(){

}

void ChargerStateMachine::ChargingWithVentilationState(){

}

void ChargerStateMachine::FaultState(){

}

void ChargerStateMachine::ErrorState(){

}

void ChargerStateMachine::RegisterStates(){

}