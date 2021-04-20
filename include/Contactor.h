#pragma once

class Contactor{
    public:
        /**
         * @brief Initializes the Conactor functionality
         */
        static void Init();

        /**
         * @brief Commands the contactor to open
         */
        static void Open();

        /**
         * @brief Commands the contactor to close
         */
        static void Close();

        /**
         * @brief Checks if the contactor is closed
         * 
         * @return true if the contactor is closed
         * @return false if the contactor is open
         */
        static bool IsClosed();
};