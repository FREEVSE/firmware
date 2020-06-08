#include <Configuration.h>
#include <Preferences.h>

Preferences Configuration::prefs = Preferences();

void Configuration::Init(){
    prefs.begin("config", false);
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
size_t Configuration::GetWiFiSSID(char *ssid){
    return prefs.getString("WIFI_SSID", ssid, CFG_SIZE_WIFI_SSID);
}

void Configuration::SetWiFiSSID(const char * ssid){
    prefs.putString("WIFI_SSID", ssid);
}

size_t Configuration::GetWiFiPass(char *pass){
    return prefs.getString("WIFI_PASS", pass, CFG_SIZE_WIFI_PASS);
}

void Configuration::SetWiFiPass(const char * pass){
    prefs.putString("WIFI_PASS", pass);
}

//Control Pilot PWM signal duty cycle
//Get
int Configuration::GetCpPwmDutyCycle(){
    float onTime = Configuration::GetMaxOutputAmps() / CP_PWM_AMP_STEP * CP_PWM_STEP;
    float period = 1 / CP_PWM_FREQ;
    return 1023 - int(onTime / period * 1023);
}