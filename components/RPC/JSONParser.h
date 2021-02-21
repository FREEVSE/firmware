#include <MessageParser.h>
#include <ArduinoJson.h>

class JSONParser : MessageParser<DynamicJsonDocument>{
    public:
        const char *Serialize(DynamicJsonDocument msg) override;
        DynamicJsonDocument Deserialize(const char *msg) override;
};