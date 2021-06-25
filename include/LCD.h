#pragma once

#ifndef NO_LCD
#include <LiquidCrystal_I2C.h>
#endif

#define LCD_COLS 20
#define LCD_ROWS 4
#define LCD_SPACE_SYMBOL 0x20

#define LCD_ICON_WIFI_ON 0
#define LCD_ICON_WIFI_OFF 1

class LCD{
    public:
        /**
         * @brief Initializes the LCD and related functionality
         */
        static void Init();

        /**
         * @brief Prints the current status to the upper left hand of the LCD
         * @details The status can occupy a maximum of 18 characters to leave room for the BT and WiFi icons
         * @param status String to print
         */
        static void PrintStatus(const char * status);

        /**
         * @brief Set the Wifi icon
         * 
         * @param state 
         * - True for connected
         * - False for disconnected
         */
        static void SetWifiState(bool state);

        /**
         * @brief Prints the charger's capabilities to the lower line on the LCD
         * @details Max amperage is printed left aligned, L1/L2 and voltage is printed right aligned
         * @param amps Amperage to print
         * @param l1 True if FREEVSE is L1, false if L2
         */
        static void PrintCapabilities(u_short amps, bool l1);

        static void PrintDebugInfo();

        /**
         * @brief Starts the timer on the LCD
         */
        static void StartTimer();

        /**
         * @brief Stops the timer on the LCD
         */
        static void StopTimer();

        static void ClearTimer();
    private:
        #ifndef NO_LCD
        static TaskHandle_t timerTask;
        static LiquidCrystal_I2C device;
        static void TimerTask(void * params);
        #endif
};