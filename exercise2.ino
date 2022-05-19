/**
    CS2600 - Final
    Jeremy Embar
    
    Exercise 2
    MQTT structure from https://www.emqx.com/en/blog/esp32-connects-to-the-free-public-mqtt-broker
    Thermistor wiring from Freenove example project.
*/

#define PIN_LED       2
#define PIN_ANALOG_IN 34
#define PIN_BUTTON    12

#include <WiFi.h>
#include <PubSubClient.h>

// WiFi
const char *ssid = "NETGEAR-50"; // wifi name (removed in submission)
const char *password = "aquaticcheese691#";  // wifi password (removed in submission)

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io"; // using public EMQX broker
const char *topic = "cs2600final/exercise2";
const char *mqtt_username = "user";
const char *mqtt_password = "pass";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  // Connect to the wifi
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to wifi");
    delay(1000); // refresh every second
  }

  Serial.println("Connected to wifi");
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  // Loop to connect the client until it's connected
  while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Connected");
    }
    else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      Serial.println();
      delay(1000);
    }
  }

  Serial.println("Starting");
  client.subscribe(topic);

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);
}

void callback(char *topic, byte *payload, unsigned int length) {
  // Create copies since they are overwritten during callback
  char *topic_copy = (char *) malloc(100);
  memcpy(topic_copy, topic, 100);
  char input[100];
  memcpy(input, payload, length);
  int adcValue = analogRead(PIN_ANALOG_IN);
  double voltage = (float)adcValue / 4095.0 * 3.3;
  double Rt = 10 * voltage / (3.3 - voltage);
  double tempK = 1 / (1 / (273.15 + 25) + log(Rt / 10) / 3950.0);
  double tempC = tempK - 273.15;
  Serial.printf("ADC value : %d,\tVoltage : %.2fV, \tTemperature : %.2fC\n", adcValue, voltage, tempC);

  Serial.println(input);

  if (strcmp(input, (char *)"Temperature") == 0) {
    Serial.println("Sending the temperature:");
    delay(500);
    char *output = (char *) malloc(100);
    dtostrf(tempC, 0, 2, output);
    client.publish(topic_copy, output);
    free(output);
  }

  else if(input[0] != '0') {
    Serial.println("Received a signal to set the timer");
    int timer = (int) ((15.0 - ((tempC - 20.0) / 3.0)) * 60000.0);
    Serial.print("Set timer for ");
    Serial.println(timer);
    delay(timer);
    digitalWrite(PIN_LED, HIGH);
  }

  // Free memory for the copy
  free(topic_copy);
}

void loop() {
  client.loop();
  if (digitalRead(PIN_BUTTON) == LOW && digitalRead(PIN_LED) == HIGH) {
    digitalWrite(PIN_LED, LOW);
    client.publish("cs2600final/exercise2", "Temperature");
    delay(1000);
  }
}
