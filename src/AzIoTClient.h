#ifndef AZIOTCLIENTINITIALIZATION_H
#define AZIOTCLIENTINITIALIZATION_H

#include <Arduino.h>
#include <mqtt_client.h>
#include <az_core.h>
#include <az_iot.h>
#include <string>

#include "AzIoTConfigOffline.h"
#include "AzIoTMethodHandlers.h"
#include "AzIoTSasToken.h"
#include "SerialLogger.h"

#define sizeofarray(a) (sizeof(a) / sizeof(a[0]))
#define LED_YELLOW 33
#define MQTT_QOS1 1
#define DO_NOT_RETAIN_MSG 0
#define SAS_TOKEN_DURATION_IN_MINUTES 60
#define INCOMING_DATA_BUFFER_SIZE 128
#define DIRECT_METHOD_NAME_SIZE 64

static char incoming_data[INCOMING_DATA_BUFFER_SIZE];
static char direct_method_name[DIRECT_METHOD_NAME_SIZE];
static char response_topic[INCOMING_DATA_BUFFER_SIZE];
static char response_data[] = "{\"isEnabled\":0,\"autoMode\":0,\"R\":\"000\",\"G\":\"000\",\"B\":\"000\"}";

static const char* host = IOT_CONFIG_IOTHUB_FQDN;
static const char* mqtt_broker_uri = "mqtts://" IOT_CONFIG_IOTHUB_FQDN;
static const char* device_id = IOT_CONFIG_DEVICE_ID;
static const int mqtt_port = AZ_IOT_DEFAULT_MQTT_CONNECT_PORT;

static char mqtt_client_id[128];
static char mqtt_username[128];
static char mqtt_password[200];
static uint8_t sas_signature_buffer[256];
static unsigned long next_telemetry_send_time_ms = 0;
static char telemetry_topic[128];
static uint8_t telemetry_payload[100];
static uint32_t telemetry_send_count = 0;

esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event);

void initializeIoTHubClient();

int initializeMqttClient();

void azIoTClientLoop();

#endif //AZIOTCLIENTINITIALIZATION_H
