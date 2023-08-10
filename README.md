# Grid Management System - Device and Firmware
## IoT class system of electricity management based on a cloud interface
The project involves the design and development of a device that allows remote control the power supply of the connected receiver. Powering the receiver on and off should be possible on demand, after a certain period of time, during designated periods. In addition, the device will conduct measurements of voltage and current and count the energy used (Smart Socket). The second part of the project is creation of a web application to manage the device and present statistics. Communication with the device and data storage will be possible through a configured cloud service.
## Technologies
* C++
* Azure IoT Hub
* Azure Stream Analytics
* ESP-32
## Libraries
* SPIFFS
* LiquidCrystal
* ESPAsyncWebServer
* EmonLib
* Azure IoT device SDK for C
## Device features
* Wi-Fi connection wizzard
* 230V/10A power supply control
* Manual mode, time mode, auto mode
* Programmable display (Voltage and current, date, power, power consumption)
* Power compsumption counter with auto-resetter
* Sending telemetry to cloud
## Circuit idea diagram
![idea](/diagram/Idea.png)
## Circuit diagram
![circuit](/device/circuit/gms.png)
## PCB design
![pcb](/device/circuit/pcb.jpg)
![pcb_preview](/device/circuit/pcb_preview.jpg)
## Device case design
![case_top](/device/case/render/top.png)
![case_bottom](/device/case/render/bottom.png)
## System architecture
![architecture](/diagram/Architektura.png)
The solution consists of six major components:
* IoT Device - smart device able to work in GMS.
* Azure IoT Hub - a managed cloud service that acts as a central message hub for communication between an IoT application and its attached devices.
* Azure Stream Analytics - cloud service that provides real-time analytic computations on the data streaming from IoT devices.
* Azure Database for PostgreSQL - Database used to store devices telemetry and web app data.
* Azure App Service - Web app used for device manipulation and telemetry presentation. [(Backend)](https://github.com/wybieracz/GMS-App)
* Azure Storage Accounts - Web app used for device manipulation and telemetry presentation. [(Frontend)](https://github.com/wybieracz/GMS-App)
## Getting started
1. Create IoT device using circuit diagram / PCB design.
2. Create account on [Microsoft Azure](https://azure.microsoft.com/en-gb/).
3. Create [Azure IoT Hub](https://docs.microsoft.com/en-gb/azure/iot-hub/iot-hub-create-through-portal).
4. In Azure IoT Hub add new device via `Devices > Add Device`.
5. Clone repository.
6. Open `AzIoTConfig.h` file.
* In Azure IoT Hub open `Overview` to show hostname.
* Enter cloud hostname `IOTHUB_FQDN`.
* In Azure IoT Hub open `Devices > Your Device Name` to show cloud device credentials.
* Enter cloud device credentials, Device ID `DEVICE_ID` and Primary Key `DEVICE_KEY`.
7. Build and upload programme to ESP-32.
8. Create Postgres database.
9. Run [backend](https://github.com/wybieracz/GMS-App) to create entities.
10. Add manually data about devices into devices table.
11. Create and configure Azure Stream Analytics.
* Define Azure IoT Hub as an input in `Inputs > Add input`.
* Define Postgres database as an output in `Outputs > Add output`.
* Define query:
```
SELECT
    [deviceId],
    [voltage],
    [current],
    [power],
    [kWh],
    [timestamp]
INTO
    [database_name]
FROM
    [iot_hub_name]
```
12. Connect power supply to device.
13. Connect to the Wi-Fi network. Credentials will be displayed on device display.
14. Go to `192.168.4.1` and pass credentials to your home Wi-Fi. The device will reboot and connect to the Wi-Fi.
15. Your device is now online and can be linked with your account.
16. For device manipulation check [GMS - App](https://github.com/wybieracz/SZEE-App).
## Device
![device_front](/device/photos/front.jpg)
![device_top](/device/photos/top.jpg)
![device_inside](/device/photos/inside.jpg)
Device overview.

![device_wifi](/device/photos/wifi.jpg)
After device startup it will show Wi-Fi credentials. When connected, it is possible to configure device netowrk settings and connect it to the home Wi-Fi.

![device_connect](/device/photos/connect.jpg)
After network configuration, device will reboot and connect to the home Wi-Fi.

![device_display_1](/device/photos/display_1.jpg)
![device_display_2](/device/photos/display_2.jpg)
It is possible to configure data presented on device display. 16x2 LCD allows to display two of data: Time and date, voltage and current, power, power comsumption. In addition it is powwible to change display brightness.
## Video presentation
[YouTube](https://youtu.be/LqCKxmR3ckY)
