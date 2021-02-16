#pragma once

#include <LiquidCrystal_I2C.h>

#define LCD_COLS 20
#define LCD_ROWS 4
#define LCD_SPACE_SYMBOL 0x20

#define LCD_ICON_WIFI_ON 0
#define LCD_ICON_WIFI_OFF 1

class LCD{
    public:
        static void Init();
        static LiquidCrystal_I2C device;
        static void PrintStatus(const char * status);
        static void SetWifiState(bool state);        
        static void PrintCapabilities(u_short amps, bool l1);
        static void StartTimer();
        static void StopTimer();
    private:
        static TaskHandle_t timerTask;
        static void TimerTask(void * params);
};