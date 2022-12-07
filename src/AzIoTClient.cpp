#include <azure_ca.h>
#include "AzIoTClient.h"

static az_iot_hub_client client;
static esp_mqtt_client_handle_t mqtt_client;
static AzIoTSasToken sasToken(
  &client,
  AZ_SPAN_FROM_STR(DEVICE_KEY),
  AZ_SPAN_FROM_BUFFER(mqttPassword),
  AZ_SPAN_FROM_BUFFER(sasSignatureBuffer)
);

static esp_err_t mqttEventHandler(esp_mqtt_event_handle_t event) {

  switch (event->event_id) {
    int i, r, rid;
    char* ptr;
    uint16_t status;

    case MQTT_EVENT_ERROR:
      logger.info("MQTT event MQTT_EVENT_ERROR");
      break;
      
    case MQTT_EVENT_CONNECTED:
      logger.info("MQTT event MQTT_EVENT_CONNECTED");
      r = esp_mqtt_client_subscribe(mqtt_client, AZ_IOT_HUB_CLIENT_METHODS_SUBSCRIBE_TOPIC, 1);

      if (r == -1) logger.error("Could not subscribe for direct methods messages.");
      else logger.info("Subscribed for direct methods messages; message id:"  + String(r));
      break;

    case MQTT_EVENT_DISCONNECTED:
      digitalWrite(LED_YELLOW, LOW);
      logger.info("MQTT event MQTT_EVENT_DISCONNECTED");
      break;

    case MQTT_EVENT_SUBSCRIBED:
      logger.info("MQTT event MQTT_EVENT_SUBSCRIBED");
      logger.info("Message ID:" + (String)event->msg_id);
      break;

    case MQTT_EVENT_UNSUBSCRIBED:
      logger.info("MQTT event MQTT_EVENT_UNSUBSCRIBED");
      break;

    case MQTT_EVENT_PUBLISHED:
      logger.info("MQTT event MQTT_EVENT_PUBLISHED");
      break;

    case MQTT_EVENT_DATA:
      logger.info("MQTT event MQTT_EVENT_DATA");
      logger.info("Message ID:" + (String)event->msg_id);

      for (i = 0; i < (INCOMING_DATA_BUFFER_SIZE - 1) && i < event->topic_len; i++) {
        incomingData[i] = event->topic[i]; 
      }
      incomingData[i] = '\0';
      logger.info("Topic: " + String(incomingData));

      //Get method name from topic. Works only when subscribed "$iothub/methods/POST/#"
      i = 0;
      ptr = incomingData + 21;

      while (*ptr && *ptr != '/') {
        methodName[i] = *ptr;
        i++;
        ptr++;
      }

      methodName[i] = '\0';
      logger.info("Method: " + String(methodName));

      while (*ptr && *ptr != '=') {
        ptr++;
      }
      ptr++;

      logger.info("rid: " + String(ptr));
      
      for (i = 0; i < (INCOMING_DATA_BUFFER_SIZE - 1) && i < event->data_len; i++) {
        incomingData[i] = event->data[i]; 
      }

      incomingData[i] = '\0';
      logger.info("Data: " + String(incomingData));

      if(String(methodName).equals("enableTelemetry")) {
        enableTelemetry(incomingData[0]) ? status = 200 : status = 400;
        iotClient.sendResponse(az_span_create_from_str(ptr), status, NULL, 0);
      }
      else if(String(methodName).equals("setDisplay")) {
        lcdManager.setDisplay(incomingData) ? status = 200 : status = 400;
        iotClient.sendResponse(az_span_create_from_str(ptr), status, NULL, 0);
      }
      else if(String(methodName).equals("setBrightness")) {
        lcdManager.setBrightness(incomingData) ? status = 200 : status = 400;
        iotClient.sendResponse(az_span_create_from_str(ptr), status, NULL, 0);
      }
      else if(String(methodName).equals("setReset")) {
        energyManager.setReset(incomingData[0]) ? status = 200 : status = 400;
        iotClient.sendResponse(az_span_create_from_str(ptr), status, NULL, 0);
      }
      else if(String(methodName).equals("setPeriod")) {
        energyManager.setPeriod(incomingData) ? status = 200 : status = 400;
        iotClient.sendResponse(az_span_create_from_str(ptr), status, NULL, 0);
      }
      else if(String(methodName).equals("setMode")) {
        incomingData[event->data_len - 1] = '\0';
        relayManager.setMode(incomingData + 1) ? status = 200 : status = 400;
        iotClient.sendResponse(az_span_create_from_str(ptr), status, NULL, 0);
      }
      else if(String(methodName).equals("getStatus")) {
        if(energyManager.avgV == 0.0) iotClient.sendResponse(az_span_create_from_str(ptr), 425, NULL, 0);
        else {
          az_span devStatus = AZ_SPAN_FROM_BUFFER(iotClient.payloadBuffer);
          iotClient.getStatusPayload(devStatus, &devStatus);
          iotClient.sendResponse(az_span_create_from_str(ptr), 200, (char*)az_span_ptr(devStatus), az_span_size(devStatus));
        }
      }
      else if(String(methodName).equals("setSettings")) {
        energyManager.setReset(incomingData[3]) ? status = 200 : status = 400;
        incomingData[3] = '\0';
        if(status == 200) energyManager.setPeriod(incomingData + 1) ? status = 200 : status = 400;
        if(status == 200) lcdManager.setDisplay(incomingData + 4) ? status = 200 : status = 400;
        incomingData[event->data_len - 1] = '\0';
        if(status == 200) lcdManager.setBrightness(incomingData + 6) ? status = 200 : status = 400;
        az_span devStatus = AZ_SPAN_FROM_BUFFER(iotClient.payloadBuffer);
        iotClient.getStatusPayload(devStatus, &devStatus);
        iotClient.sendResponse(az_span_create_from_str(ptr), status, (char*)az_span_ptr(devStatus), az_span_size(devStatus));
      }
      else if(String(methodName).equals("setRegistered")) {
        iotClient.changeRegisterState(incomingData[0]) ? status = 200 : status = 400;
        if(incomingData[0] == 48) {
          iotClient.sendResponse(az_span_create_from_str(ptr), status, "0", 1);
          ESP.restart();
        } else if(incomingData[0] == 49) {
          iotClient.sendResponse(az_span_create_from_str(ptr), status, "1", 1);
        }
      }
      else {
        status = 404;
      }
      logger.info("Code: " + String(status));
      break;

    case MQTT_EVENT_BEFORE_CONNECT:
      logger.info("MQTT event MQTT_EVENT_BEFORE_CONNECT");
      break;

    default:
      logger.error("MQTT event UNKNOWN");
      break;
  }
  return ESP_OK;
}

void AzIoTClient::sendResponse(az_span rid, uint16_t status, char * payload, int size) {

  if (az_result_failed(az_iot_hub_client_methods_response_get_publish_topic(
    &client, rid, status, responseTopic, sizeof(responseTopic), NULL))) {
    logger.error("Failed to get response topic!");
    return;
  }

  if (esp_mqtt_client_publish(
      mqtt_client,
      responseTopic,
      payload,
      0,
      MQTT_QOS1,
      DO_NOT_RETAIN_MSG) == -1
  ) {
    logger.error("Failed to response direct method!");
  }
  else {
    logger.info("Responsed to direct method.");
  }
}

void AzIoTClient::initIoTHubClient() {

  if (az_result_failed(az_iot_hub_client_init(
      &client,
      az_span_create((uint8_t*)host, strlen(host)),
      az_span_create((uint8_t*)deviceId, strlen(deviceId)),
      NULL)
  )) {
    logger.error("Failed initializing Azure IoT Hub client");
    return;
  }

  size_t client_id_length;
  
  if (az_result_failed(az_iot_hub_client_get_client_id(&client, mqttClientId, sizeof(mqttClientId) - 1, &client_id_length))) {
    logger.error("Failed getting client id");
    return;
  }

  if (az_result_failed(az_iot_hub_client_get_user_name(&client, mqttUsername, sizeofarray(mqttUsername), NULL))) {
    logger.error("Failed to get MQTT clientId, return code");
    return;
  }

  logger.info("Client ID: " + String(mqttClientId));
  logger.info("Username: " + String(mqttUsername));
}

int AzIoTClient::initMqttClient() {

  if (sasToken.Generate(SAS_TOKEN_DURATION_IN_MINUTES) != 0) {
    logger.error("Failed generating SAS token");
    return 1;
  }

  esp_mqtt_client_config_t mqtt_config;
  memset(&mqtt_config, 0, sizeof(mqtt_config));
  mqtt_config.uri = mqttBrokerUri;
  mqtt_config.port = mqttPort;
  mqtt_config.client_id = mqttClientId;
  mqtt_config.username = mqttUsername;
  mqtt_config.password = (const char*)az_span_ptr(sasToken.Get());
  mqtt_config.keepalive = 30;
  mqtt_config.disable_clean_session = 0;
  mqtt_config.disable_auto_reconnect = false;
  mqtt_config.event_handle = mqttEventHandler;
  mqtt_config.user_context = NULL;
  mqtt_config.cert_pem = (const char*)ca_pem;

  mqtt_client = esp_mqtt_client_init(&mqtt_config);

  if (mqtt_client == NULL) {
    logger.error("Failed creating mqtt client");
    return 1;
  }

  esp_err_t start_result = esp_mqtt_client_start(mqtt_client);

  if (start_result != ESP_OK) {
    logger.error("Could not start mqtt client; error code:" + start_result);
    return 1;
  }
  else {
    logger.info("MQTT client started; output code:" + (int)start_result);
    digitalWrite(LED_YELLOW, HIGH);
    return 0;
  }
}

void AzIoTClient::getTelemetryPayload(az_span payload, az_span* out_payload) {

  char* date = timeManager.getDataString(false, true);
  az_span original_payload = payload;

  payload = az_span_copy(payload, AZ_SPAN_FROM_STR("{ \"deviceId\": \""));
  payload = az_span_copy(payload, AZ_SPAN_FROM_STR(DEVICE_ID));
  payload = az_span_copy(payload, AZ_SPAN_FROM_STR("\", \"voltage\": "));
  (void)az_span_dtoa(payload, energyManager.avgV, 4, &payload);
  payload = az_span_copy(payload, AZ_SPAN_FROM_STR(", \"current\": "));
  (void)az_span_dtoa(payload, energyManager.avgI, 4, &payload);
  payload = az_span_copy(payload, AZ_SPAN_FROM_STR(", \"power\": "));
  (void)az_span_dtoa(payload, energyManager.avgP, 4, &payload);
  payload = az_span_copy(payload, AZ_SPAN_FROM_STR(", \"kWh\": "));
  (void)az_span_dtoa(payload, energyManager.kWh, 8, &payload);
  payload = az_span_copy(payload, AZ_SPAN_FROM_STR(", \"timestamp\": \""));
  payload = az_span_copy(payload, az_span_create_from_str(date));
  payload = az_span_copy(payload, AZ_SPAN_FROM_STR("\" }"));
  payload = az_span_copy_u8(payload, '\0');
  *out_payload = az_span_slice(original_payload, 0, az_span_size(original_payload) - az_span_size(payload) - 1);
}

void AzIoTClient::getStatusPayload(az_span payload, az_span* out_payload) {

  char* date = timeManager.getDataString(false, true);
  az_span original_payload = payload;

  payload = az_span_copy(payload, AZ_SPAN_FROM_STR("{ \"deviceId\": \""));
  payload = az_span_copy(payload, AZ_SPAN_FROM_STR(DEVICE_ID));
  payload = az_span_copy(payload, AZ_SPAN_FROM_STR("\", \"voltage\": "));
  (void)az_span_dtoa(payload, energyManager.avgV, 4, &payload);
  payload = az_span_copy(payload, AZ_SPAN_FROM_STR(", \"current\": "));
  (void)az_span_dtoa(payload, energyManager.avgI, 4, &payload);
  payload = az_span_copy(payload, AZ_SPAN_FROM_STR(", \"power\": "));
  (void)az_span_dtoa(payload, energyManager.avgP, 4, &payload);
  payload = az_span_copy(payload, AZ_SPAN_FROM_STR(", \"kWh\": "));
  (void)az_span_dtoa(payload, energyManager.kWh, 8, &payload);
  payload = az_span_copy(payload, AZ_SPAN_FROM_STR(", \"relayState\": "));
  (void)az_span_i32toa(payload, (int)relayManager.state, &payload);
  payload = az_span_copy(payload, AZ_SPAN_FROM_STR(", \"mode\": "));
  (void)az_span_i32toa(payload, relayManager.mode, &payload);
  payload = az_span_copy(payload, AZ_SPAN_FROM_STR(", \"rules\": \""));
  payload = az_span_copy(payload, az_span_create_from_str(relayManager.rules));
  payload = az_span_copy(payload, AZ_SPAN_FROM_STR("\", \"reset\": "));
  (void)az_span_i32toa(payload, energyManager.reset, &payload);
  payload = az_span_copy(payload, AZ_SPAN_FROM_STR(", \"periodStart\": "));
  (void)az_span_i32toa(payload, energyManager.periodStart, &payload);
  payload = az_span_copy(payload, AZ_SPAN_FROM_STR(", \"brightness\": "));
  (void)az_span_i32toa(payload, lcdManager.brightness, &payload);
  payload = az_span_copy(payload, AZ_SPAN_FROM_STR(", \"lcdSettings\": \""));
  payload = az_span_copy(payload, az_span_create_from_str(lcdManager.settings));
  payload = az_span_copy(payload, AZ_SPAN_FROM_STR("\", \"timestamp\": \""));
  payload = az_span_copy(payload, az_span_create_from_str(date));
  payload = az_span_copy(payload, AZ_SPAN_FROM_STR("\" }"));
  payload = az_span_copy_u8(payload, '\0');
  *out_payload = az_span_slice(original_payload, 0, az_span_size(original_payload) - az_span_size(payload) - 1);
}

void AzIoTClient::sendTelemetry() {

  az_span telemetry = AZ_SPAN_FROM_BUFFER(payloadBuffer);

  logger.info("Sending telemetry ...");

  // The topic could be obtained just once during setup,
  // however if properties are used the topic need to be generated again to reflect the
  // current values of the properties.
  if (az_result_failed(az_iot_hub_client_telemetry_get_publish_topic(&client, NULL, topicBuffer, sizeof(topicBuffer), NULL))) {
    logger.error("Failed az_iot_hub_client_telemetry_get_publish_topic");
    return;
  }
  
  getTelemetryPayload(telemetry, &telemetry);
  
  if (esp_mqtt_client_publish(
          mqtt_client,
          topicBuffer,
          (const char*)az_span_ptr(telemetry),
          az_span_size(telemetry),
          MQTT_QOS1,
          DO_NOT_RETAIN_MSG)
      == 0) {
    logger.error("Failed publishing");
  } else {
    logger.info("Message published successfully");
  }
}

void AzIoTClient::init() {
  registeredMem = flashManager.read(REGISTERED_PATH);
  if(registeredMem == "1") registered = true;
  else registered = false;
  initIoTHubClient();
  (void)initMqttClient();
}

void AzIoTClient::check() {

  if (sasToken.IsExpired()) {
    digitalWrite(LED_YELLOW, LOW);
    logger.info("SAS token expired! Reconnecting with a new one.");
    (void)esp_mqtt_client_destroy(mqtt_client);
    initMqttClient();
  } else if (millis() > nextTelemetryTime) {
    if(!energyManager.isFirstRead()) {
      energyManager.checkPeriod();
      energyManager.calcAvg();
      if (registered) sendTelemetry();
    }
    
    nextTelemetryTime = millis() + TELEMETRY_FREQUENCY_MILLISECS;
  }
}

bool AzIoTClient::changeRegisterState(int state) {
  if(state == 49) {
    registered = true;
    flashManager.write(REGISTERED_PATH, "1");
    return true;
  }
  
  if(state == 48) {
    registered = false;
    flashManager.write(SSID_PATH, "");
    flashManager.write(PASS_PATH, "");
    flashManager.write(RELAY_PATH, "");
    flashManager.write(LCD_BRIGHTNESS_PATH, "");
    flashManager.write(LCD_SETTINGS_PATH, "");
    flashManager.write(KWH_PATH, "");
    flashManager.write(RESET_PATH, "");
    flashManager.write(PERIOD_PATH, "");
    flashManager.write(LAST_DAY_PATH, "");
    flashManager.write(MODE_PATH, "");
    flashManager.write(RULES_PATH, "");
    flashManager.write(REGISTERED_PATH, "0");
    return true;
  }
  return false;
}

AzIoTClient iotClient;
