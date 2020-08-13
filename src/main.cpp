#include "Arduino.h"
#include <Wire.h>
#include <DHT.h>
#include <EEPROM.h>
#include <CommandHandler.h>
#include <Commands.h>
#include <ChargerAtm.h>

#include <Configuration.h>

//Hardware modules
#include <GFCI.h>
#include <ControlPilot.h>
#include <ACDetector.h>
#include <LCD.h>

//#include <WiFiManager.h>

DHT* dht;
CommandHandler<> serialCommandHandler = CommandHandler<>();
ChargerAtm stateMachine;


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
  serialCommandHandler.AddCommand(F("set"), Cmd_Set);
  serialCommandHandler.AddCommand(F("WiFi_Connect"), Cmd_WiFiConnect);

  //GFCI
  serialCommandHandler.AddCommand(F("GFCI"), Cmd_GFCI);
  //ACDetector
  serialCommandHandler.AddCommand(F("AC"), Cmd_AC);
  
  serialCommandHandler.AddCommand(F("ReadSettings"), Cmd_ReadSettings);
}

void setup() {
  pinMode(32, OUTPUT);

  pinMode(CTR_ENER_PIN, OUTPUT);

  //Set analog read res to a lower value for slightly more linear curve.
  analogReadResolution(9);

  //Set I2C pins
  Wire.begin(SDA_PIN, SCL_PIN);

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

  Serial.println(" - Initializeing AC detector...");
  ACDetector::Init();

  //Init met sensor
  /*Serial.println(" - Initializing environmental sensor...");
  dht = new DHT(DHT_PIN, DHT22);
  dht->begin();
  
  if(!dht->read()) {//Test DHT sensor
    Serial.println("  -> !!! Error initializing met sensor !!!");
  }*/

  //Reset the GFCI
  GFCI::Reset();

  stateMachine.trace(Serial);
  stateMachine.begin();

  //ControlPilot::BeginPulse();

  //WiFiManager::Init();

  LCD::Init();
  LCD::SetWifiState(true);
}

void loop() {
  serialCommandHandler.Process();

  stateMachine.cycle();

  delay(100);
}

