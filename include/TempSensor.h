#pragma once
#include "DHT.h"

class TempSensor{
    public:
        static bool Init();
        static float ReadTemp();
    private:
        static DHT dht;

};