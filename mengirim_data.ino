
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <MovingAverage.h>

const char* ssid = "asa";
const char* password = "........";
const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient;                                   
PubSubClient client(espClient); 
MovingAverage <uint8_t, 5> filter; 

#define MSG_BUFFER_SIZE  (50)

unsigned long now = millis();

void setup_wifi() {                                     //melakukan koneksi ke wifi

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
}

void callback(char* topic, byte* payload, unsigned int length) {  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);

  } else {
    digitalWrite(BUILTIN_LED, HIGH);
  }
}


void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.publish("nodered-elka", "Dah tersambung nih...");    //mengirim pesan "terkoneksi" dengan topik "nodered-elka"
      client.subscribe("receiver-elka");                            //subscribe ke topik "sender-elka"
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  unsigned long currentMillis = millis(); 
  if (currentMillis - now >= 500) {
    now = currentMillis;
  String data ="";
  while (Serial.available()>0){
    data += char(Serial.read());
  }
  data.trim();
  String strTemp = String(data);
  Serial.println(strTemp.c_str());
  client.publish("nodered-elka", strTemp.c_str());        
  }
}
