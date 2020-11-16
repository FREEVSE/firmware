#include <LCD.h>
//#include <LiquidCrystal_I2C.h>

#define LCD_CLEAR_STATUS\
    device.setCursor(0, 0);\
    device.print("                   ")

LiquidCrystal_I2C LCD::device(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

const byte wifiOnIcon[8] = { 0x00, 0x00, 0x0E, 0x11, 0x04, 0x0A, 0x00, 0x04 };
const byte wifiOffIcon[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04 };

void LCD::Init(){
    while (device.begin(LCD_COLS, LCD_ROWS) != 1) //colums - 20, rows - 4
    {
        Serial.println(F("PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,16,11,12,13,14 are legal."));
        delay(5000);   
    }

    device.createChar(LCD_ICON_WIFI_ON, wifiOnIcon);
    device.createChar(LCD_ICON_WIFI_OFF, wifiOffIcon);
}

void LCD::PrintStatus(const char *status){
    LCD_CLEAR_STATUS;
    device.setCursor(0, 0);
    device.print(status);
}

void LCD::SetWifiState(bool connected){
    device.setCursor(20, 0);

    if(connected) { device.write(LCD_ICON_WIFI_ON); }
    else { device.write(LCD_ICON_WIFI_OFF); }
}

void LCD::PrintCapabilities(u_short amps, bool l1){
    device.setCursor(0, 3);
    device.print("Max:");
    device.print(amps);
    device.print("A");
    
    device.setCursor(13 , 3);
    if(l1){ device.print("L1/120V"); }
    else{ device.print("L2/240V"); }
}

void LCD::StartChargeTimer(float secs = 0){

}