#pragma once

class ACDetector{
    public:
        static void Init();
        static bool IsL1Present();
        static bool IsL2Present();

    private:
        static void IRAM_ATTR DetectISR(void *);
        static bool L1Detected;
        static bool L2Detected;

        volatile static unsigned long lastL1Detection;
        volatile static unsigned long lastL2Detection;
};