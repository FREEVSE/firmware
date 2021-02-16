#pragma once

class Bluetooth{
    public:
        static void Init();
        static void ELMTask(void * params);
        static void printError();
};
