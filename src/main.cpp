#include "Arduino.h"
#include <Wire.h>
#include <EEPROM.h>
#include <ChargerAtm.h>
#include <Common.h>

#include <Configuration.h>

//Hardware modules
#include <Contactor.h>
#include <GFCI.h>
#include <ControlPilot.h>
#include <ACDetector.h>
#include <LCD.h>
#include <TempSensor.h>

#include <WiFiManager.h>
#include <Bluetooth.h>

ChargerAtm stateMachine;

#define LOG_TAG "main"

void setup() {

  //Set I2C pins
  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.begin(115200);
  LOG_I("FREEVSE v%s", FREEVSE_VERSION);
  LOG_I(" - © 2019 Matthew Goulart");
  LOG_I("Beginning initialization...");
  LOG_I(" - Loading configuration data from flash memory...");  

  //Configuration
  Configuration::Init();
  //PWM for max amperage
  LOG_I("  -> Maximum supply current set to %dA", Configuration::GetMaxOutputAmps()); 

  #ifdef EN_CONTACTOR
  LOG_I(" - Initializing Contactor...");
  Contactor::Init();
  #endif

  #ifdef EN_GFCI
  LOG_I(" - Initializing GFCI...");
  GFCI::Init();
  #endif

  #ifdef EN_CP
  LOG_I(" - Initializing control pilot...");
  ControlPilot::Init();
  #endif

  #ifdef EN_AC_DETECTOR
  LOG_I(" - Initializeing AC detector...");
  ACDetector::Init();
  #endif

  #ifdef EN_LCD
  LOG_I(" - Initializing LCD...");
  LCD::Init();
  LCD::SetWifiState(false);
  #endif

  #ifdef EN_TEMP_SENSOR
  LOG_I(" - Initializing environmental sensor...");
  if(!TempSensor::Init()) {
    LOG_E("  -> !!! Error initializing met sensor !!!");
  }
  #endif

  stateMachine.trace(Serial);
  stateMachine.begin();

  WiFiManager::Init();
  Bluetooth::Init();
}

void loop() {  
  stateMachine.cycle();

  delay(100);
}

