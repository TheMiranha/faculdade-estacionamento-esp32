#include <WiFi.h>
#include <String.h>
#include <PubSubClient.h>

// LEDS
#define R 32
#define G 33
#define Y 2

// SENSORS
#define TR 18
#define EC 19

const char* parkId = "15d98963-f193-4758-9afe-da499a324490";
const char* buyTopic = "/park/buy/15d98963-f193-4758-9afe-da499a324490";

const String wifi_ssid = "Fernando";
const String wifi_password = "fernando190";

const char* mqtt_broker = "192.168.15.10";
const char* mqtt_username = "username";
const char* mqtt_password = "password";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

bool isOwned = false;
bool hasObject = false;


void prepareWifi(){
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("[WiFi]: connected: "+ wifi_ssid);
}

void prepareMqtt() {
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(mqtt_callback);

  while (!client.connected()) {
    String clientId = parkId;
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("[MQTT]: connected");
    } else {
      Serial.println("[MQTT]: error on connect");
      Serial.println(client.state());
      delay(2000);
    }
  }

  client.subscribe(buyTopic);
}

void setup() {
  Serial.begin(115200);

  pinMode(R, OUTPUT);
  pinMode(Y, OUTPUT);
  pinMode(G, OUTPUT);

  pinMode(TR, OUTPUT);
  pinMode(EC, INPUT);

  prepareWifi();
  prepareMqtt();
}

void loop() {
  client.loop();
  useSensors();
  useLeds();
}

void mqtt_callback(char *topic, byte *payload, unsigned int length) {
  String content = "";
  for (int i = 0; i < length; i++) {
    content = content + (char)payload[i];
  }
  if (content == "true") {
    isOwned = true;
  } else {
    isOwned = false;
  }
}

int sensor_error_calc = 0;

void useSensors() {
  digitalWrite(TR, HIGH);
  delayMicroseconds(10);
  digitalWrite(TR, LOW);

  float distance, duration;

  duration = pulseIn(EC, HIGH);

  distance = 0.017 * duration;

  if (distance < 5) {
    if (sensor_error_calc == 10) {
      hasObject = true;
    } else {
      sensor_error_calc++;
    }
  } else {
    if (sensor_error_calc == -10) {
      hasObject = false;
    } else {
      sensor_error_calc--;
    }
  }
}

void useLeds() { 
  if (isOwned) {
    digitalWrite(R, LOW);
    digitalWrite(G, LOW);
    digitalWrite(Y, HIGH);
    return;
  }

  if (hasObject) {
    digitalWrite(R, HIGH);
    digitalWrite(G, LOW);
    digitalWrite(Y, LOW);
  } else {
    digitalWrite(R, LOW);
    digitalWrite(G, HIGH);
    digitalWrite(Y, LOW);
  }
}
