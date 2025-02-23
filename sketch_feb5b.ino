#include <Wire.h>
#include <LiquidCrystal.h>
#include <DHT11.h>

#define I2C_SLAVE_ADDR 8  // I2C address for ESP32
#define LDR_PIN A3        // LDR sensor connected to A3
#define POT_PIN A1        // Potentiometer connected to A1
#define RED_LED 7         // Red LED pin
#define GREEN_LED 8       // Green LED pin
#define BUZZER 10          // Buzzer pin

DHT11 dht11(6);  // DHT11 connected to pin 6

// Initialize LCD with correct pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);  // RS, E, D4, D5, D6, D7

void setup() {
  Wire.begin(I2C_SLAVE_ADDR);

  Serial.begin(9600);

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();

  // Set LED and Buzzer pins as OUTPUT
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // Default state: Green LED ON, Red LED OFF, Buzzer OFF
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);
}

void loop() {
  int temperature = 0, humidity = 0;
  int result = dht11.readTemperatureHumidity(temperature, humidity);

  int ldrValue = analogRead(LDR_PIN);  // Read LDR raw value (0-1023)

  lcd.clear();
  if (result == 0) {
    // Display temperature and humidity on LCD
    lcd.setCursor(0, 0);
    lcd.print("T:" + String(temperature) + "C H:" + String(humidity) + "%");

    // Display LDR & Potentiometer values
    lcd.setCursor(0, 1);
    lcd.print("LDR:" + String(ldrValue/10) + "%");

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print("°C | Humidity: ");
    Serial.print(humidity);
    Serial.print("% | LDR: ");
    Serial.print(ldrValue);

    // 🚨 Check conditions for Red LED & Buzzer activation
    if ((ldrValue > 900 || ldrValue < 200) || 
        (humidity > 50 || humidity < 30) || 
        (temperature > 29 || temperature < 20)) {
      
      // Activate Warning: Red LED ON, Green OFF, Buzzer ON
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, LOW);
      buzzer();
      
      Serial.println("⚠ WARNING: Unsafe Conditions!");
    } else {
      // Normal Conditions: Green LED ON, Red OFF, Buzzer OFF
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(RED_LED, LOW);
      digitalWrite(BUZZER, LOW);
    }
  } else {
    lcd.setCursor(0, 0);
    lcd.print("DHT11 Error");
    Serial.println("DHT11 Error");
  }

   String message;
  if (result == 0) {
    message = "T:" + String(temperature) + " H:" + String(humidity) + "% LDR:" + String(ldrValue/10);
  } else {
    message = "DHT Error";
  }

  Wire.beginTransmission(I2C_SLAVE_ADDR);
  Wire.write(message.c_str(), message.length());
  Wire.endTransmission();

  // Debugging
  Serial.println("Sent data to ESP32: " + message);

  delay(500);  // Update every 2 seconds
}


void buzzer(){
   digitalWrite(BUZZER, HIGH);
   delay(500);
  digitalWrite(BUZZER, LOW);
  delay(50);
  digitalWrite(BUZZER, HIGH);
   delay(500);
  digitalWrite(BUZZER, LOW);
  delay(50);
}
