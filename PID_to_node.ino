
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>
#include <PID_v2.h>

// Set the OneWire bus pin
#define ONE_WIRE_BUS 2

// Set the PID parameters
double Kp = 2;
double Ki = 5;
double Kd = 1;
double input = 0; 
double output = 0;
// Set the setpoint temperature
double _setpoint = 35;

// Set the PID limits
double pidMin = 0;
double pidMax = 100;

// Set the sample time for the PID controller
unsigned long sampleTime = 1000;

// Set the MQTT broker details
const char* ssid = "asa";
const char* password = "........";
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_topic = "nodered-elka";


// Define the OneWire and DallasTemperature objects
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Define the PID object
PID myPID(&input, &output, &_setpoint, &Kp, &Ki, &Kd, DIRECT);

// Define the MQTT client object
WiFiClient espClient;
PubSubClient client(espClient);

// Define the variables for PID control

void setup() {
  // Start the serial communication
  Serial.begin(9600);

  // Start the DallasTemperature object
  sensors.begin();

  // Initialize the PID object
  myPID.SetOutputLimits(pidMin, pidMax);
  myPID.SetSampleTime(sampleTime);
  myPID.SetMode(AUTOMATIC);

  // Connect to the MQTT broker
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  // Request the temperature from the DS18B20 sensor
  sensors.requestTemperatures();

  // Read the temperature from the sensor
  double temperature = sensors.getTempCByIndex(0);

  // Compute the PID output based on the temperature
  Input = temperature;
  myPID.Compute();
  double pid_output = output;

  // Convert the temperature and PID output to strings
  String temp_string = String(temperature);
  String pid_string = String(pid_output);

  // Publish the temperature and PID output to the MQTT broker
  client.connect(espClient);
  client.publish(mqtt_topic, (temp_string + "," + pid_string).c_str());
  client.disconnect();

  // Wait for a few seconds before taking another reading
  delay(5000);
}
