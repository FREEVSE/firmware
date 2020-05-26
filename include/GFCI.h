#define GFCI_ST_SET true
#define GFCI_ST_TRIPPED false

class GFCI{
    public:
        static void Init();
        static void Reset();
        static bool State();
};