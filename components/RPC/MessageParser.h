#pragma once

template <class Tmsg>
class MessageParser{
    public:
        virtual const char *Serialize(Tmsg msg);
        virtual Tmsg Deserialize(const char *msg);
};