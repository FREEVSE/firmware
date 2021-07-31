#pragma once

class Bluetooth{
    public:
        /**
         * @brief Initializes the BT stack, and services
         * @details This will:
         * - Initialize a BLE server
         * - If WiFi isn'connected, it will initialize blufi
         */
        static void Init();
        static void ScanForObdAdapter();
};
