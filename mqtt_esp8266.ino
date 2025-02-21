#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "GalaxyM31";
const char* password = "chamath365";

// MQTT Broker details
const char* mqtt_server = "214ac66723df4d4d98e975377ed679b2.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;

// MQTT Credentials
const char* mqtt_user = "python_test";
const char* mqtt_pass = "622676nhP";

// MQTT Topics
const char* main_topic = "sensor/data";      // Single topic
const char* temp_topic = "sensor/temp";      // Subtopic for temperature
const char* humidity_topic = "sensor/humidity"; // Subtopic for humidity
const char* ldr_topic = "sensor/ldr";        // Subtopic for LDR

WiFiClientSecure espClient;
PubSubClient client(espClient);

#define I2C_SLAVE_ADDR 8  // I2C address of ESP32 (slave)

// Buffer for receiving data
String receivedData = "";
bool dataReceived = false;

void setup() {
  Serial.begin(115200);

  // Initialize I2C as slave
  Wire.begin(I2C_SLAVE_ADDR);
  Wire.onReceive(receiveData);  // Register callback function for data reception

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");

  // Set up MQTT client
  espClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);
  
  connectToMQTT();
}

void loop() {
  // If new data has been received, publish it to the MQTT broker
  if (dataReceived) {
    Serial.println("Received Data: " + receivedData);

    // Publish the full data to MQTT
    client.publish(main_topic, receivedData.c_str());

    // Optionally, parse the data and send separate topics
    String tempData = parseValue(receivedData, "T:");
    String humidityData = parseValue(receivedData, "H:");
    String ldrData = parseValue(receivedData, "LDR:");

    if (tempData.length() > 0) {
      client.publish(temp_topic, tempData.c_str());
    }
    if (humidityData.length() > 0) {
      client.publish(humidity_topic, humidityData.c_str());
    }
    if (ldrData.length() > 0) {
      client.publish(ldr_topic, ldrData.c_str());
    }

    // Clear receivedData after publishing
    receivedData = "";
    dataReceived = false;  // Reset flag to indicate data has been processed
  }
  
  // Keep the MQTT client connected
  if (!client.connected()) {
    connectToMQTT();
  }
  client.loop();

  delay(2000);  // Delay before checking for new data
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

String parseValue(String data, String prefix) {
  int startIdx = data.indexOf(prefix);
  if (startIdx != -1) {
    int endIdx = data.indexOf(' ', startIdx);
    if (endIdx == -1) {
      endIdx = data.length();  // If there is no space after the value, take the rest of the string
    }
    return data.substring(startIdx + prefix.length(), endIdx);
  }
  return "";
}

void receiveData(int byteCount) {
  // Read the incoming data from the Arduino (master)
  while (Wire.available()) {
    char receivedChar = Wire.read();
    receivedData += receivedChar;  // Append received char to the receivedData buffer
  }

  // Set the flag indicating new data has been received
  dataReceived = true;

  // Debugging
  Serial.print("Received data from Arduino: ");
  Serial.println(receivedData);
}