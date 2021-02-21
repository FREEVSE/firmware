#include <JSONParser.h>

const char *JSONParser::Serialize(DynamicJsonDocument msg){
    char out[256];
    serializeJson(msg, out);
    return out;
}

DynamicJsonDocument JSONParser::Deserialize(const char *msg){
    DynamicJsonDocument doc(256);
    deserializeJson(doc, msg);
    return doc;
}