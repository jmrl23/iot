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

Debouncer buzzerDebouncer(BUZZER_PIN, 50);

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);

  client.enableDebuggingMessages();
}

void loop() {
  client.loop();
}

void alarmSyncResponseCallback(const String &payload) {
  if (payload.equals("ON")) {
    digitalWrite(BUZZER_PIN, HIGH);
  } 
  
  if (payload.equals("OFF")) {
    digitalWrite(BUZZER_PIN, LOW);
  }
}

void onConnectionEstablished() {
  client.publish(serializeTopic("ALARM_SYNC"), "");
  client.subscribe(serializeTopic("ALARM_SYNC_RESPONSE"), alarmSyncResponseCallback);

  buzzerDebouncer.subscribe(Debouncer::Edge::FALL, [](const int state) {
    String message = state == HIGH ? "ON" : "OFF";

    Serial.println(message);

    client.publish(serializeTopic("ALARM_CHANGE"), message);
    client.publish(serializeTopic("ALARM_SYNC"), "");
  });
}

String serializeTopic(const String topic) {
  return topic + ":" + DEVICE_KEY;
}
