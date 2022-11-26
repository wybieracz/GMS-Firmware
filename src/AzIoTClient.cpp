#include <azure_ca.h>
#include "AzIoTClient.h"

static az_iot_hub_client client;
static esp_mqtt_client_handle_t mqtt_client;
static AzIoTSasToken sasToken(
  &client,
  AZ_SPAN_FROM_STR(IOT_CONFIG_DEVICE_KEY),
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

      if(String(methodName).equals("enableTelemetry")){
        enableTelemetry(incomingData[1]) ? status = 200 : status = 400;
        iotClient.sendResponse(az_span_create_from_str(ptr), status, NULL);
      }
      else if(String(methodName).equals("toggleRelay")){
        toggleRelay(incomingData[1]) ? status = 200 : status = 400;
        iotClient.sendResponse(az_span_create_from_str(ptr), status, NULL);
      }
      else if(String(methodName).equals("func3")){
        //func3(incomingData[1]) ? status = 200 : status = 400;
        //sendResponse(az_span_create_from_str(ptr), status, NULL);
      }
      else if(String(methodName).equals("func4")){
        //func4(responseData) ? status = 200 : status = 400;
        //sendResponse(az_span_create_from_str(ptr), status, responseData);
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

void AzIoTClient::sendResponse(az_span rid, uint16_t status, char * payload) {

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

  double voltage = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/250.0));
  double current = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

  az_span original_payload = payload;

  payload = az_span_copy(payload, AZ_SPAN_FROM_STR("{ \"deviceId\": \""));
  payload = az_span_copy(payload, AZ_SPAN_FROM_STR(DEVICE_ID));
  payload = az_span_copy(payload, AZ_SPAN_FROM_STR("\", \"voltage\": "));
  (void)az_span_dtoa(payload, voltage, 4, &payload);
  payload = az_span_copy(payload, AZ_SPAN_FROM_STR(", \"current\": "));
  (void)az_span_dtoa(payload, current, 4, &payload);
  payload = az_span_copy(payload, AZ_SPAN_FROM_STR(", \"timestamp\": "));
  (void)az_span_u32toa(payload, (uint32_t)time(NULL), &payload);
  payload = az_span_copy(payload, AZ_SPAN_FROM_STR(" }"));
  payload = az_span_copy_u8(payload, '\0');

  *out_payload = az_span_slice(original_payload, 0, az_span_size(original_payload) - az_span_size(payload) - 1);
}

void AzIoTClient::sendTelemetry() {

  az_span telemetry = AZ_SPAN_FROM_BUFFER(telemetryPayload);

  logger.info("Sending telemetry ...");

  // The topic could be obtained just once during setup,
  // however if properties are used the topic need to be generated again to reflect the
  // current values of the properties.
  if (az_result_failed(az_iot_hub_client_telemetry_get_publish_topic(&client, NULL, telemetryTopic, sizeof(telemetryTopic), NULL))) {
    logger.error("Failed az_iot_hub_client_telemetry_get_publish_topic");
    return;
  }

  getTelemetryPayload(telemetry, &telemetry);

  if (esp_mqtt_client_publish(
          mqtt_client,
          telemetryTopic,
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
    if (telemetryEnabled) sendTelemetry();
    nextTelemetryTime = millis() + TELEMETRY_FREQUENCY_MILLISECS;
  }
}

AzIoTClient iotClient;
