# mqtt_esp32
Green house monitoring IOT system made with mqtt server 

Main hardware used
# Arduino Uno board
# Esp32 board
# DHT11 sensor
# 16*2 LCD display
# LDR
# Buzzer
# breadboar

In this project Tmperature and the humidity of the green house is measured by the DHT11 sensor. Light intensity of the greenhouse is measured by the LDR and then it monitors the readings in the display as well as the computer display by using the MQTT server.

If one or several of the temperature,humidity and the light intensity corsses the threashold limit provided, a red color LED lights up and the buzzer rings. Simultaniously the web page also turns into red color by indicating a worning.
