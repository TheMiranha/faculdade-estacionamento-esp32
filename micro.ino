#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

#define TR1 18
#define EC1 19
#define R1 32
#define G1 33
#define Y1 2

const char* ssid = "Lucas's Galaxy S22";
const char* password = "slapo310";

String url = "http://192.168.91.165:25565";

JSONVar parks;

void prepareWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Conectado ao WiFi: ");
  Serial.println(WiFi.localIP());
}

void getParks() {
    HTTPClient http;
    String serverPath = url + "/parks";

    http.begin(serverPath.c_str());

    http.addHeader("Authorization", "Bearer IUSHEFD9PHp9h09phusadf");

    int httpResponseCode = http.GET();

    if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String response = http.getString();
        // Serial.println(response);
        
       JSONVar responseObject = JSON.parse(response);
  
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(responseObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }

      // JSONVar responseObjectKeys = responseObject.keys();
      JSONVar localParks = responseObject["parks"];
      for (int parkIndex = 0; parkIndex < localParks.length(); parkIndex++) {
        JSONVar parkObject = localParks[parkIndex];
        if (JSON.typeof(parkObject["parking"]) == "null") {
          parks[parkObject["id"]] = 0;
        } else {
          JSONVar parkingObject = parkObject["parking"];
          parks[parkObject["id"]] = 1;
        }
      }

    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
}

void setup() {
  Serial.begin(115200);

  pinMode(TR1, OUTPUT);
  pinMode(EC1, INPUT);
  
  pinMode(G1, OUTPUT);
  pinMode(R1, OUTPUT);
  pinMode(Y1, OUTPUT);

  prepareWiFi();
}

void showParks() {
  Serial.println(parks["15d98963-f193-4758-9afe-da499a324490"]);
  if (JSON.typeof(parks["15d98963-f193-4758-9afe-da499a324490"]) == "null") {
    Serial.println("Nao pronto");
    digitalWrite(G1, HIGH);
    digitalWrite(R1, HIGH);
    digitalWrite(Y1, HIGH);
    return;
  }

  int status = parks["15d98963-f193-4758-9afe-da499a324490"];
  if (status == 0) {

    digitalWrite(TR1, HIGH);
    delayMicroseconds(10);
    digitalWrite(TR1, LOW);

    float distance, duration;

    duration = pulseIn(EC1, HIGH);

    distance = 0.017 * duration;

    Serial.print("Distancia: ");
    Serial.println(distance);

    if (distance <= 2) {
      digitalWrite(G1, LOW);
      digitalWrite(R1, LOW);
      digitalWrite(Y1, HIGH);
    } else {
    digitalWrite(G1, HIGH);
    digitalWrite(R1, LOW);
    digitalWrite(Y1, LOW);
    }

    Serial.println("Nao ta reservada");
  
  } else {
    Serial.println("Reservada");
    digitalWrite(G1, LOW);
    digitalWrite(R1, HIGH);
    digitalWrite(Y1, LOW);
  }
}

void loop() {
  getParks();
  showParks();
  delay(1500);
}