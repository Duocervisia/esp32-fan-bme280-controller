#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <WiFi.h>
#include <PubSubClient.h>

#define BME_SDA 22
#define BME_SCL 21
Adafruit_BME280 bme; // I2C

const char* ssid = "WLAN";
const char* password = "20012017";
const char* mqtt_server = "homeassistant.local";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
    Serial.begin(115200);

    Wire.begin(BME_SDA, BME_SCL);
    if (!bme.begin(0x76)) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }

    setup_wifi();
    client.setServer(mqtt_server, 1883);
}

void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    // Convert temperature and humidity to string
    char tempStr[6];
    dtostrf(bme.readTemperature(), 4, 2, tempStr);
    char humStr[6];
    dtostrf(bme.readHumidity(), 4, 2, humStr);

    // Publish temperature and humidity
    client.publish("bme280/temperature", tempStr);
    client.publish("bme280/humidity", humStr);
    delay(5000);
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect, providing the username and password
        if (client.connect("ESP32Client", "user", "dcves345twaesed7hngq8w7o34zn2q83e2q")) {
            Serial.println("connected");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}