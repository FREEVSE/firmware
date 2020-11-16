#pragma once

#include <Arduino.h>

#define GFCI_ST_SET true
#define GFCI_ST_TRIPPED false

class GFCI{
    public:
        static void Init();
        static void BeginTest();
        static void EndTest();
        static bool SelfTest();
        static void Reset();
        static bool State();
};