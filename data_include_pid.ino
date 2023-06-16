#include <OneWire.h>
#include <DallasTemperature.h>
#include <PID_v2.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "asa";
const char* password = "........";
const char* mqtt_server = "test.mosquitto.org";
const char* mqtt_topic = "nodered-elka";

WiFiClient espClient;
PubSubClient client(espClient);

  double input = 0, output = 0, setpoint = 0;
  double Kp = 1, Ki = 0.5, Kd = 0.25;
PID myPID(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);

void setup() {
  Serial.begin(9600);
  delay(10);

  WiFi.begin((char*)ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  client.setServer(mqtt_server, 1883);
  client.setCallback(mqttCallback);

  myPID.SetMode(AUTOMATIC);
  myPID.SetSampleTime(1000);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  input = analogRead(A0);
  myPID.Compute();

  printAndPublishValues(input, output);

  delay(1000);
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT");
      client.publish(mqtt_topic, "Hello, world!");
      client.subscribe("receiver-elka");
    } else {
      Serial.print("Failed to connect to MQTT, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}

void mqttCallback(char* topic, byte* message, unsigned int length) {
  String payload = "";
  for (int i = 0; i < length; i++) {
    payload += (char)message[i];
  }
  setpoint = payload.toFloat();
  myPID.setpoint(setpoint);
}

void printAndPublishValues(float input, double output) {
  Serial.print("Input: ");
  Serial.print(input);
  Serial.print(" Output: ");
  Serial.println(output);
  if (client.connect(server, 1883)) {
    client.publish("input", String(input).c_str());
    client.publish("output", String(output).c_str());
    client.publish("setpoint", String(setpoint).c_str());
    client.publish("kp", String(kp).c_str());
    client.publish("ki", String(ki).c_str());
    client.publish("kd", String(kd).c_str());
}
  else {
    Serial.println("Failed to connect to MQTT broker");
}
}
