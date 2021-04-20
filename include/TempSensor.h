#pragma once
#include "DHT.h"

class TempSensor{
    public:
        /**
         * @brief Initializes the temperature sensor functionality
         * 
         * @return true If initialization succeeeded
         * @return false If initialization failed
         */
        static bool Init();

        /**
         * @brief Reads the current temperature from the sensor
         * 
         * @return float The temperature in deg C
         */
        static float ReadTemp();
    private:
        static DHT dht;

};