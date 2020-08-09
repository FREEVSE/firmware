#include <Commands.h>
#include <CommandHandler.h>
#include <Configuration.h>

#include <GFCI.h>
#include <ACDetector.h>

#include <WiFiManager.h>

const char* PROGMEM noValueError = "No value provided";
const char* PROGMEM noCommandError = "No command provided";

/**
 * Produces a (hopefully) unique value from a string
 * to be used in a switch statement. T#pragma region Debug commands

#pragma endregion
 * @version	v1.0.0	Sunday, June 21st, 2020.
 * @global
 * @param const char*   s
 * @param int           off = 0
 * @return uint     
 */
constexpr unsigned int hash(const char *s, int off = 0) {                        
    return !s[off] ? 5381 : (hash(s, off+1)*33) ^ s[off];                           
} 

#pragma region Set commands

/**
 * Handles the "set" serial command
 *
 * @author	Matthew Goulart
 * @since	v0.0.1
 * @version	v1.0.0	Sunday, June 21st, 2020.
 * @global
 * @param	commandparameter	&params	
 * @return	void
 */
void Cmd_Set(CommandParameter &params){
    const char *param = params.NextParameter();

    if(param == NULL){
        Serial.println(noValueError);
        return;
    } 

    switch (hash(param))
    {
    case hash("amps"): SetMaxAmps(params); break;
    case hash("SSID"): SetSSID(params); break;
    case hash("pass"): SetPass(params); break;
    default:
        Serial.println(F("Parameter does not exist."));
        break;
    }
}


/**
 * Sets "MaxOutputAmps" config value.
 *
 * @author	Matthew Goulart
 * @since	v0.0.1
 * @version	v1.0.0	Sunday, June 21st, 2020.
 * @global
 * @param	commandparameter	&params	
 * @return	void
 */
void SetMaxAmps(CommandParameter &params){
    int amps = params.NextParameterAsInteger();

    if(amps == NULL){
        Serial.println(noValueError);
        return;
    }

    Configuration::SetMaxOutputAmps(amps);
    Serial.print(F("Set max amp output to: "));
    Serial.println(amps);
}

/**
 * Sets "WiFiSSID" config value.
 *
 * @author	Matthew Goulart
 * @since	v0.0.1
 * @version	v1.0.0	Sunday, June 21st, 2020.
 * @global
 * @param	commandparameter	&params	
 * @return	void
 */
void SetSSID(CommandParameter &params){
    const char *ssid = params.NextParameter();

    if(ssid == NULL) {
        Serial.println(noValueError);
        return;
    }

    Configuration::SetWiFiSSID(ssid);
    Serial.print(F("Set WiFi SSID to: "));
    Serial.println(ssid);
}

/**
 * Sets "WiFiPass" config value.
 *
 * @author	Matthew Goulart
 * @since	v0.0.1
 * @version	v1.0.0	Sunday, June 21st, 2020.
 * @global
 * @param	commandparameter	&params	
 * @return	void
 */
void SetPass(CommandParameter &params){
    const char *pass = params.NextParameter();

    if(pass = NULL){
        Serial.println(noValueError);
        return;
    }

    Configuration::SetWiFiPass(pass);
    Serial.print(F("Set WiFi password to: "));
    Serial.println(pass);
}

#pragma endregion

#pragma region GFCI commands

void Cmd_GFCI(CommandParameter &params){
    const char *cmd = params.NextParameter();

    if(cmd == NULL){
        Serial.println(noCommandError);
        return;
    }

    switch(hash(cmd)){
        case hash("reset"): GFCI::Reset(); break;
        case hash("beginTest"): GFCI::BeginTest(); break;
        case hash("endTest"): GFCI::EndTest(); break;
    }
}

#pragma endregion

#pragma region ACDetector commands
void Cmd_AC(CommandParameter &params){
    const char *cmd = params.NextParameter();

    if(cmd == NULL){
        Serial.println(noCommandError);
        return;
    }

    switch(hash(cmd)){
        case hash("isL1Present"): Serial.println(ACDetector::IsL1Present()); break;
        case hash("isL2Present"): Serial.println(ACDetector::IsL1Present()); break;
    }
}

#pragma endregion
void Cmd_ReadSettings(CommandParameter &params){
    Serial.println("**Charge Settings**");
    Serial.print("Max amp output: ");
    Serial.println(Configuration::GetMaxOutputAmps());

    Serial.println("**WiFi Settings**");
    Serial.print("SSID: ");
    Serial.println(Configuration::GetWiFiSSID());

    Serial.print("Password: ");
    Serial.println(Configuration::GetWiFiPass());
}

void Cmd_WiFiConnect(CommandParameter &params){
    WiFiManager::Connect();
}