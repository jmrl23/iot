#include <EspMQTTClient.h>
#include <Debouncer.h>

#define DEVICE_KEY "EIOT-0000000001"
#define WIFI_SSID  "12345678"
#define WIFI_PASS  "87654321"
#define MQTT_HOST  "test.mosquitto.org"
#define MQTT_USER  ""
#define MQTT_PASS  ""
#define MQTT_PORT  1883

#define BUZZER_PIN D1
#define BUTTON_PIN D2

String serializeTopic(const String topic);

EspMQTTClient client(
  WIFI_SSID,
  WIFI_PASS,
  MQTT_HOST,
  MQTT_USER,
  MQTT_PASS,
  DEVICE_KEY,
  MQTT_PORT
);

Debouncer buttonDebouncer(BUTTON_PIN, 50);

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  buttonDebouncer.subscribe(Debouncer::Edge::FALL, [](const int state) {
    client.publish(serializeTopic("I_PRESS"), "");
  });

  client.enableDebuggingMessages();
}

void loop() {
  client.loop();
  buttonDebouncer.update();
}

void iSyncCallback(const String &payload) {
  if (payload.equals("ON")) {
    digitalWrite(BUZZER_PIN, HIGH);
  } 
  
  if (payload.equals("OFF")) {
    digitalWrite(BUZZER_PIN, LOW);
  }
}

void onConnectionEstablished() {
  client.publish(serializeTopic("B_SYNC"), "");
  client.subscribe(serializeTopic("I_SYNC"), alarmSyncResponseCallback);
}

String serializeTopic(const String topic) {
  return topic + ":" + DEVICE_KEY;
}
