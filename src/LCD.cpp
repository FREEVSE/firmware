#include <Common.h>
#include <Configuration.h>
#include <LCD.h>
#include <ControlPilot.h>
//#include <LiquidCrystal_I2C.h>

#define LOG_TAG "LCD"

#define LCD_CLEAR_STATUS\
    device.setCursor(0, 0);\
    device.print("                   ")

#ifdef EN_LCD
LiquidCrystal_I2C LCD::device(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);
TaskHandle_t LCD::timerTask;

const byte wifiOnIcon[8] = { 0x00, 0x00, 0x0E, 0x11, 0x04, 0x0A, 0x00, 0x04 };
const byte wifiOffIcon[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04 };
#endif

void LCD::Init(){
    #ifdef EN_LCD
    while (device.begin(LCD_COLS, LCD_ROWS) != 1) //colums - 20, rows - 4
    {
        LOG_E("PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,16,11,12,13,14 are legal.");
        delay(5000);   
    }

    device.createChar(LCD_ICON_WIFI_ON, wifiOnIcon);
    device.createChar(LCD_ICON_WIFI_OFF, wifiOffIcon);
    #endif
}

void LCD::PrintStatus(const char *status){
    #ifdef EN_LCD
    LCD_CLEAR_STATUS;
    device.setCursor(0, 0);
    device.print(status);
    #endif
}

void LCD::SetWifiState(bool connected){
    #ifdef EN_LCD
    device.setCursor(20, 0);

    if(connected) { device.write(LCD_ICON_WIFI_ON); }
    else { device.write(LCD_ICON_WIFI_OFF); }
    #endif
}

void LCD::PrintCapabilities(u_short amps, bool l1){
    #ifdef EN_LCD
    device.setCursor(0, 3);
    device.print("Max:");
    device.print(amps);
    device.print("A");
    
    device.setCursor(13 , 3);
    if(l1){ device.print("L1/120V"); }
    else{ device.print("L2/240V"); }
    #endif
}

void LCD::PrintDebugInfo(){
    std::tuple<int, int> cpValues = ControlPilot::Raw();
    device.setCursor(0, 2);
    device.printf("CP:%d/%d", std::get<0>(cpValues), std::get<1>(cpValues));
}

void LCD::StartTimer(){
    #ifdef EN_LCD
    xTaskCreatePinnedToCore(TimerTask, "TimerTask", 2048, NULL, 10, &timerTask, APP_CORE);
    #endif
}

void LCD::StopTimer(){
    #ifdef EN_LCD
    if(timerTask == NULL) return;

    vTaskDelete(timerTask);
    timerTask = NULL;
    #endif
}

void LCD::ClearTimer(){
    #ifdef EN_LCD
    device.setCursor(0, 1);
    device.print("        ");
    #endif
}

#ifdef EN_LCD
void LCD::TimerTask(void * params){
    unsigned long startTime = millis();

    while(true){
        int secs = (millis() - startTime) * 0.001;
        int mins = secs / 60;
        int hours = mins / 60;
        secs -= mins * 60;
        mins -= hours * 60;
        

        device.setCursor(0, 1);
        device.printf("%02d:%02d:%02d", hours, mins, secs);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}
#endif