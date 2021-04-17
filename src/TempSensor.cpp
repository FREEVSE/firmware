#include "TempSensor.h"
#include "Configuration.h"

DHT TempSensor::dht(DHT_PIN, DHT22);

bool TempSensor::Init(){
    #ifdef EN_TEMP_SENSOR
    dht.begin();

    return dht.read();
    #endif
}

float TempSensor::ReadTemp(){
    #ifdef EN_TEMP_SENSOR
    return dht.readTemperature();
    #else
    return 0;
    #endif
}