#pragma once

class Contactor{
    public:
        static void Init();
        static void Open();
        static void Close();
        static bool IsClosed();
};