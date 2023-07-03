#include <Q2HX711.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Network information
const char* ssid = "ssid";
const char* password = "password";

// MQTT connection information
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

// Weight sensor and HX711 information
const byte hx711_data_pin1 = 2;
const byte hx711_clock_pin1 = 4;

const byte hx711_data_pin2 = 7;
const byte hx711_clock_pin2 = 9;

Q2HX711 hx711(hx711_data_pin1, hx711_clock_pin1);
Q2HX711 hx7112(hx711_data_pin2, hx711_clock_pin2);

// WiFi and MQTT client
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);


// WiFi connection
void connectWiFi() {
  Serial.print("Connecting to: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.print("");
  Serial.println("WiFi connection done");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// MQTT broker connection
void connectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Connection to HiveMQ...");
    
    if (mqttClient.connect("arduinoClient")) {
      Serial.println("MQTT connection successfully");
    } else {
      Serial.print("MQTT connection failed. Error code: ");
      Serial.print(mqttClient.state());
      Serial.println(" Trying again in 5 secs...");
      
      delay(5000);
    }
  }
}

// Sending data function
void publishData(int s, float weight) {
  char payload[10];
  snprintf(payload, sizeof(payload), "%.2f", weight);
  if (s == 1){
    mqttClient.publish("food", payload);
  } else {
    mqttClient.publish("water", payload);
  }
  
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  connectWiFi();

  mqttClient.setServer(mqtt_server, mqtt_port);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();

  float food_read = hx711.read() / 100.0;
  float water_read = hx7112.read() / 100.0;

  float ratio1 = 84213.75 / 1;
  float ratio2 = 83886.07 / 1;

  Serial.println(food_read);
  Serial.println(water_read);

  publishData(1, food_read / ratio1);
  publishData(2, water_read / ratio2);

  delay(1000);

}
