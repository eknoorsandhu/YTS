//send data from one computer to another
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "abc";
const char* password = "abc12345";
const char* mqtt_server = "192.168.109.227";

WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_MPU6050 mpu;

void setup() {
// put your setup code here, to run once:
Serial.begin(115200);
WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("WiFi connected successfully");

client.setServer(mqtt_server, 1883);

if (!mpu.begin()) {
Serial.println("Failed to find MPU6050 chip");
while (1) {
delay(10);
}
}

mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
mpu.setGyroRange(MPU6050_RANGE_500_DEG);
mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void reconnect() {
while (!client.connected()) {
if (client.connect("ESP32Client")) {
Serial.println("Connected to MQTT broker");
} else {
delay(5000);
}
}
}

void loop() {
// put your main code here, to run repeatedly:
if (!client.connected()) {
reconnect();
}
client.loop();

sensors_event_t a, g, temp;
mpu.getEvent(&a, &g, &temp);

unsigned long currentMillis = millis();
unsigned long seconds = (currentMillis / 1000) % 60;
unsigned long minutes = (currentMillis / (1000 * 60)) % 60;
unsigned long hours = (currentMillis / (1000 * 60 * 60)) % 24;

// Format the timestamp as HH:MM
char timestamp[9]; // HH:MM
String payload = String(timestamp) + " , ";
snprintf(timestamp, sizeof(timestamp), "%02lu:%02lu:%02lu", hours, minutes, seconds);

// Construct the payload with formatted timestamp


payload += String(a.acceleration.x) + " , " + String(a.acceleration.y) + " , " + String(a.acceleration.z) +" , ";
payload += String(g.gyro.x) + " , " + String(g.gyro.y) + " , " + String(g.gyro.z);

Serial.println(payload);
client.publish("sensor/data1", (char*)payload.c_str());

delay(1000);
}