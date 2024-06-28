#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Moto G (5) Plus 1940";
const char* password = "anmol&me";


const char* mqtt_server = "";

WiFiClient espClient;
PubSubClient client(espClient);

const int emgPin = 34;
int emgValue = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("Starting setup...");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected successfully");

  // Set MQTT server
  client.setServer(mqtt_server, 1883);

  // Initialize analog read settings for EMG sensor
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  Serial.println("Setup complete.");
}

void reconnect() {
  // Reconnect to MQTT server if not connected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  // Maintain MQTT connection
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Read the EMG sensor value
  emgValue = analogRead(emgPin);
  Serial.print("EMG value: ");
  Serial.println(emgValue);

  // Get the current time
  unsigned long currentMillis = millis();
  unsigned long seconds = (currentMillis / 1000) % 60;
  unsigned long minutes = (currentMillis / (1000 * 60)) % 60;
  unsigned long hours = (currentMillis / (1000 * 60 * 60)) % 24;

  // Format the timestamp as HH:MM:SS
  char timestamp[9]; // HH:MM:SS
  snprintf(timestamp, sizeof(timestamp), "%02lu:%02lu:%02lu", hours, minutes, seconds);

  // Construct the payload with formatted timestamp and EMG data
  String payload = String(timestamp) + " , " + String(emgValue);

  // Print the payload to the serial monitor
  Serial.print("Payload: ");
  Serial.println(payload);

  // Publish the payload to the MQTT topic
  client.publish("sensor/data1", (char*)payload.c_str());

  delay(1000);
}