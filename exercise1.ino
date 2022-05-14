/**
    CS2600 - Final
    Jeremy Embar
    
    Exercise 1
    Turn the built-in ESP-32 LED off and on using 1 and 2 respectively.
    Using public EMQX MQTT broker.
    Code modified from https://www.emqx.com/en/blog/esp32-connects-to-the-free-public-mqtt-broker
*/

#include <WiFi.h>
#include <PubSubClient.h>

// WiFi
const char *ssid = "NETGEAR-50"; // wifi name (removed in submission)
const char *password = "aquaticcheese691#";  // wifi password (removed in submission)

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io"; // using public EMQX broker
const char *topic = "cs2600final/exercise1";
const char *mqtt_username = "user";
const char *mqtt_password = "pass";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(2, OUTPUT); // Built-in LED pin

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  // Connect to the wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); // refresh every 0.5 seconds
  }

  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  // Loop to connect the client until it's connected
  while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      Serial.println();
      delay(2000);
    }
  }

  client.publish(topic, "test");
  client.subscribe(topic);
}

void callback(char *topic, byte *payload, unsigned int length) {
  if (length == 1) { // message is the correct length
    if ((char) payload[0] == '1') {
      Serial.print("Received a 1, turing the LED off.");
      Serial.println();
      digitalWrite(2, LOW);
    }
    else if ((char) payload[0] == '2') {
      Serial.print("Received a 2, turing the LED on.");
      Serial.println();
      digitalWrite(2, HIGH);
    }
  }
}

void loop() {
  client.loop();
}
