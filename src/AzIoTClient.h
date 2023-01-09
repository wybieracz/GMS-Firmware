#ifndef AZIOTCLIENTINITIALIZATION_H
#define AZIOTCLIENTINITIALIZATION_H

#include <Arduino.h>
#include <mqtt_client.h>
#include <az_core.h>
#include <az_iot.h>
#include <string>

#include "AzIoTConfigOffline.h"
#include "AzIoTSasToken.h"
#include "SerialLogger.h"
#include "PinManager.h"
#include "LcdManager.h"
#include "EnergyManager.h"

#define sizeofarray(a) (sizeof(a) / sizeof(a[0]))
#define MQTT_QOS1 1
#define DO_NOT_RETAIN_MSG 0
#define SAS_TOKEN_DURATION_IN_MINUTES 60
#define INCOMING_DATA_BUFFER_SIZE 256
#define OUTCOMING_DATA_BUFFER_SIZE 128
#define DIRECT_METHOD_NAME_SIZE 64

static esp_err_t mqttEventHandler(esp_mqtt_event_handle_t event);

static char incomingData[INCOMING_DATA_BUFFER_SIZE];
static char methodName[DIRECT_METHOD_NAME_SIZE];
static char responseTopic[OUTCOMING_DATA_BUFFER_SIZE];

static const char* host = IOTHUB_FQDN;
static const char* mqttBrokerUri = "mqtts://" IOTHUB_FQDN;
static const char* deviceId = DEVICE_ID;
static const int mqttPort = AZ_IOT_DEFAULT_MQTT_CONNECT_PORT;

static char mqttClientId[128];
static char mqttUsername[128];
static char mqttPassword[200];
static uint8_t sasSignatureBuffer[256];

class AzIoTClient {
  private:
    unsigned long nextTelemetryTime = START_TELEMETRY_FREQUENCY_MILLISECS;
    char topicBuffer[128];
    String registeredMem;
    bool registered;

    void getTelemetryPayload(az_span payload, az_span* out_payload);
    void initIoTHubClient();
    int initMqttClient();
    void sendTelemetry();

  public:
    uint8_t payloadBuffer[512];
    bool changeRegisterState(int state);
    void sendResponse(az_span rid, uint16_t status, char * payload, int size);
    void getStatusPayload(az_span payload, az_span* out_payload);
    void init();
    void check();
};

extern AzIoTClient iotClient;

#endif //AZIOTCLIENTINITIALIZATION_H
