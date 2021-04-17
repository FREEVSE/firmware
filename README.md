This repository contains the source code and documentation for the FREEVSE firmware. Below you will find information on modifying, building and using the firmware. Contributions are welcome and encouraged.
## Features
<small>Checked items are implemented, unchecked are planned.</small>
### General Features
 - [x] Bluetooth WiFi provisioning
 - [x] OTA updates (https)
 - [x] Remote monitoring and control via HTTP rpc server
 - [ ] Vehicle/charge monitoring via OBDII link
 - [ ] Vehicle history and statics sent to backend server (optional)
### Safety Features
 - [x] Continuous protected earth monitoring
 - [x] Single/Double phase monitoring
 - [x] Stuck contactor warning
 - [ ] Temperature monitoring
 - [ ] Contactor cycle count

## Overview
The firmware is written for the esp-idf with arduino as a component. It makes use of several FREERTOS features such as tasks. All EVSE related work is done on a single task, in a state machine. Other work, such as bluetooth and wifi is performed on separate tasks.

In general, hardware features are represented in code by a class. Functions of the hardware are static methods of this class. 
### Safety
#### GFCI
GFCI monitoring is linked to an interrupt. It is important to note that **the esp has absolutely no influence on the behaviour of the GFCI module** other than to reset it in the event that it trips. All official FREEVSE boards are made with a *completely* independent GFCI module. This ensures that the most critical safety component remains functional even with firmware updates potentially introducing bugs.

#### Phase detection and monitoring (L1/L2)
The firmware monitors for the presence of 1 or 2 phases. On boot, the firmware will check how many phases are present and set itself to either L1 or L2 mode. In L1 mode, only one phase is monitored. In L2 mode, both phases are monitored and, should a phase drop out, charging is stopped and retried as per J1772.

#### Contactor monitoring
The phase detection feature is used for determining if the contactor closes and opens when requested. If the contactor is commanded closed and there isn't at least 1 phase present, the firmware will warn the user that the contactor is stuck, and power should be cut off at the breaker. The opposite is also true; if the contactor was commanded open but voltage remains at the output, the same message will appear.

#### Temperature monitoring (not yet implemented)
The firmware will monitor the ambient temperature inside the FREEVSE enclosure. If the temperature rises above a certain limit, the output will be derated until a maximum cutoff temperature at which point charging will stop until the temperature falls to acceptable limits. This will occur a maximum of 3 times at which point charging will be suspended until the next unplug-plug cycle.

#### Safety Flow Diagram
The diagram assumes a vehicle is plugged in when the EVSE is powered on. 
![enter image description here](https://mermaid.ink/svg/eyJjb2RlIjoiZ3JhcGggVEJcbkEoW1Bvd2VyIE9uXSkgLS0-IEJbU2VsZiBUZXN0XVxuQiAtLSBmYWlsIC0tPiBDKFtMb2Nrb3V0XSlcbkIgLS0gcGFzcyAtLT4gRFtCZWdpbiBDaGFyZ2luZ11cbkQgLS0-IEVbTW9uaXRvciBUZW1wXVxuRCAtLT4gRltNb25pdG9yIEwxL0wyXVxuRCAtLT4gR1tNb25pdG9yIEdGQ0ldXG5FIC0tIG92ZXIgbGltaXQgLS0-IEhbZGVyYXRlIG91dHB1dF1cbkggLS0-IEVcbkggLS0gb3ZlciBtYXggLS0-IENcbkYgLS0gZmFpbCAtLT4gQ1xuRyAtLSBmYWlsIC0tPiBJW1JldHJ5IDMgdGltZXMgLSBub3QgaW1wbGVtZW50ZWQgeWV0XVxuSSAtLT4gR1xuSSAtLSBtYXggcmV0cmllcyAtLT4gQyIsIm1lcm1haWQiOnsidGhlbWUiOiJkZWZhdWx0In0sInVwZGF0ZUVkaXRvciI6ZmFsc2V9)
#### Vehicle
FREEVSE firmware will support generic OBDII adapters so information can be read and optionally sent to backend storage. The idea is to allow vehicles with no communication capabilities to still provide rich data to the EVSE. This would allow things like stopping a charge at a certain %, monitoring battery health and energy consumption calculations.

#### Controlling a FREEVSE
When a FREEVSE is first booted up, it will advertise itself over BLE. Using the BluFi protocol from espressif, one would input wifi credentials. At this point, the unit will connect to wifi and will reconnect if rebooted.

The FREEVSE uses mdns, allowing it to be found on the network by anyone that knows what to look for. This means that **the security of the WiFi network is the only security in place for the FREEVSE**. If you don't want strangers to have access you should have a secure WiFi network.

The firmware runs a simple, custom HTTP RPC server. It allows for `getting` and `setting` values as well as executing functions with arguments. Once the api is more mature, it will be laid out here.

#### Updates
The FREEVSE, by default, will try to update itself with an HTTPS server (configured in the firmware) every 12 hours. This can be disabled entirely, or you can run your own backend server. You just need to change the appropriate `#define`s in `configuration.h`

## Building
Building the firmware should be pretty easy thanks to Platformio. Just clone the repo and open it in vscode (or other). Thats it!
One caveat is that platformio doesn't support the latest IDF version so we need to specify a version of the esp arduino framework that is compatible. This is configured in `platformio.ini`

