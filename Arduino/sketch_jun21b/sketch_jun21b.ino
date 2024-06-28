#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <PubSubClient.h>


// WiFi and MQTT server credentials
const char* ssid = "my hotspot";
const char* password = "12345678";
const char* mqtt_server = "192.168.220.196";

// WiFi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);

// EMG sensor pin
const int emgPin = 34;
int emgValue = 0;

// MPU6050 sensor
Adafruit_MPU6050 mpu;

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

  // Initialize EMG sensor
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  // Initialize MPU6050 sensor
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(1000);
    }
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

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

  // Read MPU6050 sensor values
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Get the current time
  

  // Construct the payload with timestamp, EMG data, and MPU6050 data
  String payload = "EMG: " + String(emgValue) + ", ";
  payload += "Accel: [" + String(a.acceleration.x) + ", " + String(a.acceleration.y) + ", " + String(a.acceleration.z) + "], ";

  

  // Print the payload to the serial monitor
  Serial.print("Payload: ");
  Serial.println(payload);

  // Publish the payload to the MQTT topic
  client.publish("sensor/data", (char*)payload.c_str());

  delay(1000);
}