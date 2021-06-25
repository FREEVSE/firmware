#pragma once

#include <Preferences.h>
#include <BLEUtils.h>

/**
 * Development switches
 * These are "shortcuts" for setting the firmware up for common dev scenarios
 */

//#define DEBUG
//#define OCD //Disables any peripherals using pins needed for OCD
//#define NO_POST //Disables power on self test. Will default to 120v mode.
//#define NO_LCD //Won't init LCD
//#define NO_SAFETY_CHECKS //Disables monitoring of protected earth and ambient temp

/**
 * Versions
 * These are the various versions associated with this firmware build
 */

//Firmware version
#define FREEVSE_VERSION "1.0.0"

//Hardware version
#define FREEVSE_BOARD "1.0.0"

/**
 * Update settings
 * These settings control automatic and manual updates
 * To disable updates entiery, comment EN_UPDATES. This will prevent both
 * automatic AND manual updates.
 */
#define EN_UPDATES
#define FREEVSE_SERVER "freevse.org"
#define FREEVSE_SERVER_SCHEME "https"
#define UPDATE_INTERVAL_MS 4.32e+7
#define UPDATE_RETY_COUNT 3

//Modules
//Comment the line of the module you want to disable
/**
 * Hardware Modules
 * Commenting any of these lines will disable the associated hardware module.
 * Some functions will return "dummy" values in order for the rest of the firmware
 * to continue functioning. 
 */
#define EN_AC_DETECTOR
#define EN_CP
#define EN_CONTACTOR
#define EN_LCD
#define EN_TEMP_SENSOR
#define EN_GFCI

/**
 * Pin assignments
 */
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

/**
 * CP State Levels
 * These define the ADC values used for determining the state of the CP.
 * These will probably work for all ESPs regardless of ADC calibration since
 * the ranges are relatively broad. They can be tweaked if you run an ADC calibration.
 * 
 * Since analog read resolution is set to 9, there are 512 possible values.
 * The CP varies between -12 (0) and +12v (511).
 */

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
#define CP_LOW_MAX CP_STATE_TOLERANCE

/**
 * PWM configuration
 * The EVSE equipment communicates the amperage it can provide with PWM
 * on a 1kHz carrier using the following equation: Imax = 0.6A * (D / 10us)
 * where D is the duty cycle in μs. So for each 10μs of "on time", add 0.6 amps.
 */

#define CFG_MAX_AMP_DEFAULT 12  //12A is generally safe for any 15A outlet. Some chargers use 8A.

#define CP_PWM_FREQ 1000.0      //PWM freq as per SAE J1772. (needs a decimal point for float arithmetic)
#define CP_PWM_PERIOD_US 1 / CP_PWM_FREQ * 1000000
#define CP_PWM_AMP_STEP 0.6     //Amps per step of "high" time as per SAE J1772
#define CP_PWM_STEP 0.00001     //Duration of one "step" as per SAE J1772

/**
 * Misc settings
 * You probably shouldn't change any of these unless you know
 * what you are doing.
 */
//Cores
#define APP_CORE 1

//Timers
#define CP_TIMER 0
#define GFCI_TIMER 1

//GFCI configuration
#define GFCI_PWM_CHAN 2
#define GFCI_TST_FREQ 2000

#include <ConfigurationBackend.h>

#define BLE_ADDR_LEN 6
#define MAX_BLE_DEVICES 8
#define BLE_DEV_1 1
#define BLE_DEV_2 2
#define BLE_DEV_3 3
#define BLE_DEV_4 4
#define BLE_DEV_5 5
#define BLE_DEV_6 6
#define BLE_DEV_7 7
#define BLE_DEV_8 8



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

        //BLE devices
        static BLEAddress GetBleDeviceAddr(ushort ev);
        static bool SetBleDeviceAddr(ushort dev, BLEAddress addr);
        static bool ClearBleDeviceAddr(ushort dev);
        static void GetBleDeviceAddrs(BLEAddress* addrs);

        static void Save();

        static Preferences prefs;
        static Preferences bleDev;

    private:
        
        static short _maxOutputAmps;

};