#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#define I2C_SLAVE_ADDR 8  // Arduino's I2C address

// WiFi credentials
const char* ssid = "GalaxyM31";
const char* password = "chamath365";

// MQTT Broker details
const char* mqtt_server = "214ac66723df4d4d98e975377ed679b2.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;  // Secure MQTT port

// MQTT Credentials
const char* mqtt_user = "python_test";  
const char* mqtt_pass = "622676nhP";  

WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  Wire.begin();  // Initialize I2C as Master (ESP32)

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");

  // Use insecure SSL (for quick testing)
  espClient.setInsecure();

  // Set MQTT server
  client.setServer(mqtt_server, mqtt_port);

  connectToMQTT();
}

void connectToMQTT() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32_Client", mqtt_user, mqtt_pass)) {  
      Serial.println("Connected to MQTT broker!");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(". Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    connectToMQTT();
  }
  client.loop();

  // Request data from Arduino via I2C
  Wire.requestFrom(I2C_SLAVE_ADDR, 32);  // Request up to 32 bytes

  String receivedData = "";
  while (Wire.available()) {
    char c = Wire.read();
    receivedData += c;
  }

  if (receivedData.length() > 0) {
    Serial.print("Received from Arduino: ");
    Serial.println(receivedData);

    // Publish received data to MQTT topic "test"
    client.publish("test", receivedData.c_str());
  }

  delay(2000);  // Wait before next request
}
