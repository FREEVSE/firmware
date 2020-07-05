#include <Arduino.h>

#define GFCI_ST_SET true
#define GFCI_ST_TRIPPED false

class GFCI{
    public:
        static void Init();
        static void BeginTest();
        static void EndTest();
        static void Reset();
        static bool State();
        static void Inhibit(int ms = 1000);
    private:
        static hw_timer_t *intTimer;
        static void IRAM_ATTR EndInhibit();
};