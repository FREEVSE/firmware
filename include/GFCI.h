#pragma once

#include <Arduino.h>

#define GFCI_ST_SET true
#define GFCI_ST_TRIPPED false

/**
 * @brief Class representing the hardware GFCI
 * @details This class plays an incredibly import role in keeping
 * the FREEVSE safe to use. That is, it plays no role. All official
 * FREEVSE boards have a completely independent GFCI subcircuit. The
 * firmware can only reset the GFCI after it trips. That means that even
 * if the ESP is completely frozen, the FREEVSE hardware will still cut power to the
 * vehicle if a ground fault occurs.
 */
class GFCI{
    public:
        /**
         * @brief Initializes the GFCI functionality
         */
        static void Init();

        /**
         * @brief Begins testing the GFCI subcircuit
         * @details Sends a square wave through the GFCI current transformer.
         * Since current transformers work best with pure sine waves, a frequency and duration is
         * chosen such that the GFCI trips as fast as possible. This can take some time with a square wave.
         */
        static void BeginTest();

        /**
         * @brief Ends testing the GFCI subcircuit
         */
        static void EndTest();

        /**
         * @brief Performs a self test of the GFCI subcircuit
         * 
         * @return true If the GFCI passes the test
         * @return false If the GFCI fails the test
         */
        static bool SelfTest();

        /**
         * @brief Resets the GFCI
         * @details Pulses the GFCI latch "reset" pin. It may refuse to reset,
         * so State() should be checked.
         */
        static void Reset();

        /**
         * @brief Returns the state of the GFCI
         * 
         * @return true If not tripped
         * @return false If tripped
         */
        static bool State();
};