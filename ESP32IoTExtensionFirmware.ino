#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Wi-Fi Information
const char* WIFI_SSID = "";
const char* WIFI_PASSWORD = "";

// MQTT Information
const char* MQTT_HOST = "";
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC = "mit-appinventor/esp32/internal/<YOUR IDENTIFIER HERE>";

// PubSubClient Setup
WiFiClient wifiClient;
PubSubClient mqtt_client(wifiClient);

StaticJsonDocument<128> json_doc;

void setup(){
	Serial.begin(115200);

	// Connect to Wi-Fi
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	Serial.print("Connecting to Wi-Fi...");
	while(WiFi.status() != WL_CONNECTED){
		delay(150);
		Serial.print(".");
	}
	Serial.println("");
	Serial.print("Connected with IP ");
	Serial.println(WiFi.localIP());

	// Setup MQTT Broker
	mqtt_client.setServer(MQTT_HOST, MQTT_PORT);
	mqtt_client.setCallback(mqtt_callback);
}

void loop(){
	if(!mqtt_client.connected()){
		mqtt_reconnect();
	}
	mqtt_client.loop();
}

void mqtt_reconnect(){
	while(!mqtt_client.connected()){
		Serial.println("Connecting to MQTT Broker...");
		if(mqtt_client.connect("App Inventor ESP32 IoT")){
			Serial.println("Connected to MQTT Broker");
			mqtt_client.subscribe(MQTT_TOPIC);
		}else{
			Serial.println("Connection failed. Waiting to try again...");
			Serial.println(mqtt_client.state());
			delay(5000);
		}
	}
}

void mqtt_callback(const char* topic, byte* payload, unsigned int length){

  Serial.print("Received message on topic ");
	Serial.println(topic);

  deserializeJson(json_doc, payload);

  const char* operation = json_doc["operation"];
  int pin = json_doc["pin"];
  bool state = json_doc["state"];

  if(strcmp(operation, "digitalWrite") == 0){
    Serial.print("Digital write ");
    Serial.print(state ? "HIGH" : "LOW");
    Serial.print(" on pin ");
    Serial.println(pin);
    pinMode(pin, OUTPUT);
    digitalWrite(pin, state ? HIGH : LOW);
  }else{
    Serial.print("Unknown operation ");
    Serial.println(operation);
  }

}