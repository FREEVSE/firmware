#include <Arduino.h>
#include <DHT.h>
#include <EEPROM.h>
#include <CommandHandler.h>
#include <Commands.h>
#include <ChargerStateMachine.h>

#include <Configuration.h>

//Hardware modules
#include <GFCI.h>
#include <ControlPilot.h>

#include <WiFiManager.h>

DHT* dht;
CommandHandler<> serialCommandHandler = CommandHandler<>();

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
  serialCommandHandler.AddCommand(F("Set_WiFi_SSID"), Cmd_SetWiFiSSID);
  serialCommandHandler.AddCommand(F("Set_WiFi_Pass"), Cmd_SetWiFiPass);
  serialCommandHandler.AddCommand(F("WiFi_Connect"), Cmd_WiFiConnect);
  serialCommandHandler.AddCommand(F("ReadSettings"), Cmd_ReadSettings);
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

  //Reset the GFCI
  GFCI::Reset();

  ChargerStateMachine::Init();

  ControlPilot::BeginPulse();

  //WiFiManager::Init();
}

void loop() {
  serialCommandHandler.Process();
  Serial.println(ControlPilot::ToString(ControlPilot::State()));

  CpState cpState = ControlPilot::State();

  //stateMachine.run();

  delay(1000);
}

