#include <Arduino.h>
#include <DHT.h>
#include <EEPROM.h>
#include <CommandHandler.h>
#include <Commands.h>

#include <Configuration.h>

//Hardware modules
#include <GFCI.h>
#include <ControlPilot.h>

#include <WiFiManager.h>

enum class State{
  Idle,
  VehicleDetected,
  Advertizing,
  Energizing,
  Charging,
  DeEnergizing,
  Fault,
  Error
};

DHT* dht;
State state;
CommandHandler<> SerialCommandHandler;

//Initializes and tests the meteorological sensor
void ReadMet(){
  
  
  float hum = dht->readHumidity();
  float temp = dht->readTemperature();

  if (isnan(hum) || isnan(temp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(hum);
  Serial.print(F("%  Temperature: "));
  Serial.print(temp);
  Serial.println(F("°C "));
}

void RegisterCommands(){
    //Set configuration variables
    SerialCommandHandler.AddCommand(F("Set_WiFi_SSID"), Cmd_SetWiFiSSID);
    SerialCommandHandler.AddCommand(F("Set_WiFi_Pass"), Cmd_SetWiFiPass);
    SerialCommandHandler.AddCommand(F("WiFi_Connect"), Cmd_WiFiConnect);
    SerialCommandHandler.AddCommand(F("ReadSettings"), Cmd_ReadSettings);
}

void setup() {
  pinMode(32, OUTPUT);

  pinMode(CTR_ENER, OUTPUT);

  analogReadResolution(9);

  delay(1000);
  Serial.begin(115200);
  Serial.println("EVSE v0.1 - © 2019 Matthew Goulart");
  Serial.println("Beginning initialization...");
  Serial.println(" - Loading configuration data from flash memory...");  

  RegisterCommands();

  //Configuration
  Configuration::Init();
  //PWM for max amperage  
  Serial.print("  -> Maximum supply current set to ");
  Serial.print(Configuration::GetMaxOutputAmps());
  Serial.println("A");

  Serial.println(" - Initializing GFCI...");
  GFCI::Init();

  Serial.println(" - Initializing control pilot...");
  ControlPilot::Init();

  //Init met sensor
  Serial.println(" - Initializing environmental sensor...");
  dht = new DHT(DHT_PIN, DHT22);
  dht->begin();

  if(!dht->read()) {//Test DHT sensor
    Serial.println("  -> !!! Error initializing met sensor !!!");
  }

  //Set initial state
  Serial.println(" - Entering idle state");
  state = State::Idle;

  //Reset the GFCI
  GFCI::Reset();

  ControlPilot::BeginPulse();

  //WiFiManager::Init();
}

void loop() {
  SerialCommandHandler.Process();
  Serial.println(ControlPilot::ToString(ControlPilot::State()));

  CpState cpState = ControlPilot::State();

  switch(state){
    //IDLE state
    //From here, we can only go to VehicleDetected
    case State::Idle:
      if(cpState == CpState::VehicleDetected){
        ControlPilot::BeginPulse();
        state = State::VehicleDetected;
      }      
    break;

    //VehicleDetected state
    //The vehicle can either remain in this state indefinitely,
    //or request charge/charge with ventilation
    //The charger could also be disconnected
    case State::VehicleDetected:
      //Vehicle requests charging
      if(cpState == CpState::Charge || cpState == CpState::ChargeWithVentilation){
        
      }
    break;
  }

  delay(1000);
}

