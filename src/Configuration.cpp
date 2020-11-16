#include <Configuration.h>
//#include <Preferences.h>

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
String Configuration::GetWiFiSSID(){
    return prefs.getString("WIFI_SSID");
}

void Configuration::SetWiFiSSID(const char * ssid){
    prefs.putString("WIFI_SSID", ssid);
}

String Configuration::GetWiFiPass(){
    return prefs.getString("WIFI_PASS");
}

void Configuration::SetWiFiPass(const char * pass){
    prefs.putString("WIFI_PASS", pass);
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

void Configuration::SetFailedUpdateCount(short count = 1){
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