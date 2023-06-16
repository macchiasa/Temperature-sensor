#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "asa";
const char* password = "........";
const char* mqtt_server = "test.mosquitto.org";         //set broker mqtt pada mosquitto

WiFiClient espClient;
PubSubClient client(espClient);

#define MSG_BUFFER_SIZE  (50)
#define RELAY D1                                        //mendefinisikan pin D1 sebagai pin Relay

unsigned long lastMsg = 0;
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {                                     //melakukan koneksi wifi

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
  Serial.print("Message arrived [");                                //menampilkan pesan yang diterima
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ((char)payload[0] == '1') {
    digitalWrite(RELAY, LOW);                                      //apabila pesan yang diterima adalah 1, maka pin Relay ON
  } else {
    digitalWrite(RELAY, HIGH);                                       //apabila pesan yang diterima selain 1, maka pin Relay OFF
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.publish("nodered-elka", "Dah tersambung nih...");     //mengirim pesan terhubung ke topik "nodered-elka"
      client.subscribe("receiver-elka");                           //subscribe ke topik "receiver-elka"
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(RELAY, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
  }
}
