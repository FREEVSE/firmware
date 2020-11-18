#pragma once

#include <Preferences.h>

//Development switches
#define DEBUG
//#define NO_POST //Disables power on self test. Will default to 120v mode.
//#define NO_SAFETY_CHECKS //Disables monitoring of protected earth and GFCI

//Firmware version
#define FREEVSE_VERSION "1.0.0"

//Hardware version
#define FREEVSE_BOARD "1.0.0"

//Update settings
#define FREEVSE_SERVER "freevse.org"
#define FREEVSE_SERVER_SCHEME "https"
#define UPDATE_RETY_COUNT 3

//Pin assignments
#define SDA_PIN 25
#define SCL_PIN 26
#define DHT_PIN 32
#define CP_PWM_PIN 15
#define CP_READ_PIN 33
#define GFI_INT_PIN 21
#define GFI_SET_PIN 18
#define GFI_TEST_PIN 16
#define CTR_ENER_PIN 19
#define SENS_L1_PIN 22
#define SENS_L2_PIN 23

//Timers
#define CP_TIMER 0
#define GFCI_TIMER 1

//CP state levels
//These are the expected ADC readings for each of the possible CP states
#define CP_WITH_VENT 294
#define CP_CHARGING 358
#define CP_PRESENT 448
#define CP_IDLE 511

#define CP_STATE_TOLERANCE 21   //CP adc value must be within +/- this value

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
#define CFG_MAX_AMP_DEFAULT 12

#define CP_PWM_FREQ 1000.0        //PWM freq as per SAE J1772. (needs a decimal point for float arithmetic)
#define CP_PWM_AMP_STEP 0.6     //Amps per step of "high" time as per SAE J1772
#define CP_PWM_STEP 0.00001      //Duration of one "step" as per SAE J1772

//GFCI configuration
#define GFCI_PWM_CHAN 2
#define GFCI_TST_FREQ 2000


class Configuration{
    public:
        static void Init();

        static short GetMaxOutputAmps();
        static void SetMaxOutputAmps(short val);
        static float GetCpPwmDutyCycle();

        //Wifi
        static String GetWiFiSSID();
        static void SetWiFiSSID(const char * ssid);

        static String GetWiFiPass();
        static void SetWiFiPass(const char * pass);

        //Updates
        static bool GetAutoUpdate();
        static void SetAutoUpdate(bool autoUpd);

        static short GetFailedUpdateCount();
        static void SetFailedUpdateCount(short count = 1);

        static void Save();

        static Preferences prefs;

    private:
        
        static short _maxOutputAmps;

};