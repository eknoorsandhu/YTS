#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// WiFi credentials
const char* ssid = "Moto G (5) Plus 1940";
const char* password = "anmol&me";

// MQTT broker address
const char* mqtt_server = "192.168.43.98";

// Objects for WiFi, MQTT, and MPU6050 sensor
WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_MPU6050 mpu;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000); // Update every minute

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected successfully");

  // Setup MQTT server
  client.setServer(mqtt_server, 1883);

  // Initialize MPU6050 sensor
  if (!mpu.begin()) {
    Serial.println("Failed to initialize MPU6050!");
    while (1){
      delay(10);
    }
  }

  // Configure sensor ranges
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // Initialize NTP client
  timeClient.begin();

  // Print current date and time
  printCurrentDateTime();
}

void reconnect() {
  // Reconnect to MQTT broker if disconnected
  while (!client.connected()) {
    Serial.println("Reconnecting....");
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT broker");
    } else {
      delay(5000);
    }
  }
}

void loop() {
  // Ensure MQTT client is connected
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Get sensor events
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  String payload = "Accelerometer: X: " + String(a.acceleration.x) + " Y: " + String(a.acceleration.y) + " Z: " + String(a.acceleration.z);
  payload += "\nGyroscope: X: " + String(g.gyro.x) + " Y: " + String(g.gyro.y) + " Z: " + String(g.gyro.z);
  payload += "\nTemperature: " + String(temp.temperature);

  Serial.println("Publishing message: ");
  Serial.println(payload);

  // Publish payload to MQTT topic
  client.publish("data/sensor2", (char*) payload.c_str());

  // Delay for 1 second
  delay(1000);
}

void printCurrentDateTime() {
  // Update NTP client
  timeClient.update();

  // Get current time
  unsigned long epochTime = timeClient.getEpochTime();
  time_t rawTime = (time_t) epochTime;
  struct tm *local = localtime(&rawTime);

  // Extract time components
  int hours = local->tm_hour;
  int minutes = local->tm_min;
  int seconds = local->tm_sec;

  // Extract date components
  int day = local->tm_mday;
  int month = local->tm_mon + 1;
  int year = local->tm_year + 1900;

  // Print local time
  if (hours < 12) {
    Serial.printf("Time is %02d:%02d:%02d am\n", hours, minutes, seconds);
  } else {
    Serial.printf("Time is %02d:%02d:%02d pm\n", hours - 12, minutes, seconds);
  }

  // Print the current date
  Serial.printf("Date is: %02d/%02d/%d\n", day, month, year);
}

