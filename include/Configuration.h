#include <Preferences.h>

#pragma once

#define DEBUG

//Pin assignments
#define DHT_PIN 13
#define CP_PWM 15
#define CP_READ 33
#define GFI_INT 23
#define GFI_SET 18
#define CTR_ENER 21

//CP state levels
//These are the expected ADC readings for each of the possible CP states
#define CP_WITH_VENT 319
#define CP_CHARGING 383
#define CP_PRESENT 460
#define CP_IDLE 511

#define CP_STATE_TOLERANCE 32   //CP adc value must be within +/- this value

//These are the possible ranges for the CP state readings. Anything within the _MIN and _MAX values will be considered valid
#define CP_WITH_VENT_MIN CP_WITH_VENT - CP_STATE_TOLERANCE
#define CP_WITH_VENT_MAX CP_WITH_VENT + CP_STATE_TOLERANCE
#define CP_CHARGING_MIN CP_CHARGING - CP_STATE_TOLERANCE
#define CP_CHARGING_MAX CP_CHARGING + CP_STATE_TOLERANCE
#define CP_PRESENT_MIN CP_PRESENT - CP_STATE_TOLERANCE
#define CP_PRESENT_MAX CP_PRESENT + CP_STATE_TOLERANCE
#define CP_IDLE_MIN CP_PRESENT_MAX

//PWM configuration
//The EVSE equipment communicates the amperage it can provide with PWM
//on a 1kHz carrier using the following equation: Imax = 0.6A * (D / 10us)
//where D is the duty cycle in μs. So for each 10μs of "on time", add 0.6 amps.

//This is the address in the EEPROM (flash) where the max amp setting is stored
//Usually this would be 80% of the breaker's amp rating.
//I.E. for a 40 amp breaker, you would set this to 32.
#define CFG_ADDR_MAX_AMP 0
#define CFG_SIZE_MAX_AMP 1
#define CFG_MAX_AMP_DEFAULT 12

#define CP_PWM_FREQ 1000.0        //PWM freq as per SAE J1772. (needs a decimal point for float arithmetic)
#define CP_PWM_AMP_STEP 0.6     //Amps per step of "high" time as per SAE J1772
#define CP_PWM_STEP 0.00001      //Duration of one "step" as per SAE J1772

//WiFi Configuration
//This just defines where wifi info is stored in flash
#define CFG_ADDR_WIFI_SSID 1
#define CFG_SIZE_WIFI_SSID 32
#define CFG_ADDR_WIFI_PASS 33
#define CFG_SIZE_WIFI_PASS 63

//This is the size needed by the configuration data
//to be stored in flash memory.
#define CFG_SIZE \
    CFG_SIZE_MAX_AMP + \
    CFG_SIZE_WIFI_SSID + \
    CFG_SIZE_WIFI_PASS

class Configuration{
    public:
        static void Init();

        static short GetMaxOutputAmps();
        static void SetMaxOutputAmps(short val);
        static int GetCpPwmDutyCycle();

        static size_t GetWiFiSSID(char *ssid);
        static void SetWiFiSSID(const char * ssid);

        static size_t GetWiFiPass(char *pass);
        static void SetWiFiPass(const char * pass);

        static void Save();

        static Preferences prefs;

    private:
        
        static short _maxOutputAmps;

};