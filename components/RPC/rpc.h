class RpcServer{
    public:
        void Init();
        void RegisterFunction(void (*func)(int));
};