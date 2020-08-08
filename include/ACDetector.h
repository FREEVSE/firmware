class ACDetector{
    public:
        static void Init();
        static bool IsL1Present();
        static bool IsL2Present();

    private:
        static void IRAM_ATTR DetectISR(void *);
        static bool L1Detected;
        static bool L2Detected;
};