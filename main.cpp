#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#define DHTPIN D5  
#define DHTTYPE    DHT11
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// Digital pin connected to the DHT sensor


// MQTT Broker
const char *mqtt_broker = "192.168.1.XX";
const char *topic = "XD_ID:ESP8266Room:xd";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
float valueT = 0; // value of Temp
float valueH = 0; // value of humidty 
unsigned long previousMillis = 0;    // will store last time DHT was updated
DHT dht(DHTPIN, DHTTYPE);
static char temp[15];
static char hum[15];

// Updates DHT readings every 10 seconds
const long interval = 10000;  

// WiFi
const char *ssid = "XDXDXDXDXDD"; // Enter your WiFi name
const char *password = "WIFIPASSSWORD";  // Enter WiFi password



void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Connecting to WiFi....");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  Serial.println();

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Room";
//    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str()) ) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_broker, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {



    lastMsg = now;

    

// MES TEM HUM 
unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;


    // Read temperature as Celsius (the default)
    float newT = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float newT = dht.readTemperature(true);
    // if temperature read failed, don't change t value
    if (isnan(newT)) {
      Serial.println("Failed to read from DHT sensor!");
      snprintf(msg,MSG_BUFFER_SIZE,"Failed to read from DHT sensor!");
      Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
    }
    else {
     valueT  = newT;
       Serial.print("Publish message: ");
       dtostrf( valueT, 3, 4, temp );
       //sprintf(msg,"TEMP: %s", temp);
    Serial.println(msg);
    client.publish("outTopic/temp", temp);
    }



    // Read Humidity
    float newH = dht.readHumidity();
    // if humidity read failed, don't change h value 
    if (isnan(newH)) {
      Serial.println("Failed to read from DHT sensor!");
      snprintf(msg,MSG_BUFFER_SIZE,"Failed to read from DHT sensor!");
      Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
    }
    else {
      valueH = newH;
 
       Serial.print("Publish message: ");
       dtostrf( valueH, 3, 4, hum );
       //sprintf(msg,"HUM: %s", temp);
    Serial.println(msg);
    client.publish("outTopic/hum", hum);
    }
  }
 /// END MES TEMP HUM

  }
  
}