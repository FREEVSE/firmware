#include <Configuration.h>
#include <WiFi.h>
//#include <Preferences.h>

#define DIGIT_TO_CHAR(digit) (char)('0' + digit)

Preferences Configuration::prefs = Preferences();
Preferences Configuration::bleDev = Preferences();

void Configuration::Init(){
    prefs.begin("config", false);
    bleDev.begin("bleDev", false);
}

//Max output amps
//Get
short Configuration::GetMaxOutputAmps(){
    return prefs.getShort("MAX_AMPS", CFG_MAX_AMP_DEFAULT);
}
//Set
void Configuration::SetMaxOutputAmps(short val){
    prefs.putShort("MAX_AMPS", val);
}

//WiFi Info
String Configuration::GetWiFiSSID(){
    //TODO: Needs to load the autoconfigured credentials
    return "NOT IMPLEMENTED";
}

void Configuration::SetWiFiSSID(const char * ssid){
    //TODO: Needs to save to the autoconfigured credentials
    return;
}

String Configuration::GetWiFiPass(){
    //TODO: Needs to load the autoconfigured credentials
    return "NOT IMPLEMENTED";
}

void Configuration::SetWiFiPass(const char * pass){
    //TODO: Needs to save to the autoconfigured credentials
    return;
}

//Updates
bool Configuration::GetAutoUpdate(){
    return prefs.getBool("AUTO_UPDATE", false);
}

void Configuration::SetAutoUpdate(bool autoUpd){
    prefs.putBool("AUTO_UPDATE", autoUpd);
}

short Configuration::GetFailedUpdateCount(){
    return prefs.getShort("FAILED_UPDATE_COUNT", 0);
}

void Configuration::SetFailedUpdateCount(short count){
    count += GetFailedUpdateCount();
    prefs.putShort("FAILED_UPDATE_COUNT", count);
}

//Control Pilot PWM signal duty cycle
//Get
float Configuration::GetCpPwmDutyCycle(){
    float period = 1 / CP_PWM_FREQ;
    float onTime = GetMaxOutputAmps() / CP_PWM_AMP_STEP * CP_PWM_STEP;
    return onTime / period;
}

//BLE devices
BLEAddress Configuration::GetBleDeviceAddr(ushort dev){
    if(dev > MAX_BLE_DEVICES){
        return NULL;
    }

    esp_bd_addr_t nativeAddr;
    char key = DIGIT_TO_CHAR(dev);

    bleDev.getBytes(&key, &nativeAddr, BLE_ADDR_LEN);

    return BLEAddress(nativeAddr);
}

bool Configuration::SetBleDeviceAddr(ushort dev, BLEAddress addr){
    if(dev > MAX_BLE_DEVICES){
        return false;
    }

    char key = DIGIT_TO_CHAR(dev);

    auto res = bleDev.putBytes(&key, addr.getNative(), BLE_ADDR_LEN);

    return res == BLE_ADDR_LEN;
}

bool Configuration::ClearBleDeviceAddr(ushort dev){
    if(dev > MAX_BLE_DEVICES){
        return false;
    }

    char key = DIGIT_TO_CHAR(dev);

    return bleDev.remove(&key);
}

void Configuration::GetBleDeviceAddrs(BLEAddress* addrs){
    for(ushort i = 0; i < MAX_BLE_DEVICES; i++){
        addrs[i] = GetBleDeviceAddr(i);
    }
}