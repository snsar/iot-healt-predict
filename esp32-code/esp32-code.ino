#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Replace with your network credentials
const char* ssid = "HUAWEI-E8372-A5B0";
const char* password = "91119941";

// Replace with your Mosquitto broker IP address
const char* mqttServer = "192.168.8.101";
const int mqttPort = 1883;
// const char* mqttUser = "your-mqtt-username";
// const char* mqttPassword = "your-mqtt-password";

#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long previousMillis = 0;
const long interval = 1000;  // Interval between sending temperature data (in milliseconds)

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Setup MQTT
  client.setServer(mqttServer, mqttPort);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    sensors.requestTemperatures();
    float tempC = sensors.getTempCByIndex(0);

    if (!client.connected()) {
      reconnect();
    }

    // Create a topic string
    String topic = "human/temperature";

    // Convert temperature to a string
    char tempStr[6];
    dtostrf(tempC, 4, 2, tempStr);

    // Publish temperature to the MQTT broker
    client.publish(topic.c_str(), tempStr);
  }
  delay(2000);
  // Other non-blocking tasks can be added here
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds");
      delay(5000);
    }
  }
}
