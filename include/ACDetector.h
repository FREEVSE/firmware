#pragma once

class ACDetector{
    public:
        /**
         * @brief Initialize the ACDetector class
         * @details Installs and ISR on the falling edge of both SENS_L*_PINs
         */
        static void Init();

        /**
         * @brief Checks if phase L1 was present in the last 60Hz cycle (the last 16.7ms)
         * 
         * @return true if the phase was detected
         * @return false if the phase was not detected
         */
        static bool IsL1Present();

        /**
         * @brief Checks if phase L2 was present in the last 60Hz cycle (the last 16.7ms)
         * 
         * @return true if the phase was detected
         * @return false if the phase was not detected
         */
        static bool IsL2Present();

    private:
        /**
         * @brief Interrupt Service Routine to handle recording when a phase is detected
         * @details The timestamp of the detection is stored in 'lastL*Detection' variables
         * 
         */
        static void IRAM_ATTR DetectISR(void *);

        /**
         * @brief Timestamp of the last L1 detection
         */
        volatile static unsigned long lastL1Detection;

        /**
         * @brief Timestamp of the last L2 detection
         */
        volatile static unsigned long lastL2Detection;
};