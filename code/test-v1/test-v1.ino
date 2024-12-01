#include <WiFiNINA.h>   
#include <PubSubClient.h>
#include <utility/wifi_drv.h>   // library to drive to RGB LED on the MKR1010
#include "arduino_secrets.h" 

#define hallPin A6   // hall sensor connected to A6
#define lightPin A0  // light sensor connected to A0

// WiFi and MQTT settings
const char* ssid          = SECRET_SSID;
const char* password      = SECRET_PASS;
const char* mqtt_username = SECRET_MQTTUSER;
const char* mqtt_password = SECRET_MQTTPASS;
const char* mqtt_server = "mqtt.cetools.org";
const int mqtt_port = 1884;

WiFiServer server(80);
WiFiClient wificlient;
WiFiClient mkrClient;
PubSubClient client(mkrClient);

// set MQTT topics and message
char mqtt_topic_color[] = "student/CASA0014/light/45/pixel/";
char mqtt_topic_brightness[] = "student/CASA0014/light/45/brightness/";

void setup() {
  Serial.begin(115200);   // Start the serial monitor to show output
  delay(1000);

  WiFi.setHostname("Lumina ucfnwut");
  startWifi();
  client.setServer(mqtt_server, mqtt_port);
  Serial.println("Setup complete");
}

void loop() {
  // make sure the arduino is connected to the MQTT broker & wifi
  // otherwise we will not receive any messagesc & connect to MQTT
  if (!client.connected()) {
    reconnectMQTT();
  }
  if (WiFi.status() != WL_CONNECTED) {
    startWifi();
  }

  // check for messages from the broker and ensuring that any outgoing messages are sent.
  client.loop(); 

  int hallValue = analogRead(hallPin); //read the analog value from the hall snesor
  Serial.print("Hall Sensor Value: ");
  Serial.println(hallValue);

  int R = 0, G = 0, B = 0;
  if (hallValue < 350) {
    // red
    R = 255; G = 0; B = 0;
  } else if (hallValue >= 350 && hallValue <= 550) {
    // yellow
    R = 255; G = 255; B = 0;
  } else {
    // green
    R = 0; G = 255; B = 0;
  }

  sendColor(R, G, B);

 
  int lightValue = analogRead(lightPin); //read the analog value from the light snesor
  Serial.print("Light Sensor Value: ");
  Serial.println(lightValue);

  //adjust brightness based on light sensor
  int brightness = 0;
  if (lightValue < 400) {
    brightness = 119; //high brightness in dim light
  } else if (lightValue >= 401 && lightValue <= 800) {
    brightness = 90;  //moderate brightness
  } else {
    brightness = 50;  //low brightness in bright light
  }

  sendBrightness(brightness);  // send brightness data to MQTT

  //delay(500); 
}

// send RGB color data to MQTT
void sendColor(int R, int G, int B) {
  char mqtt_message[100];
  for (int i = 0; i < 12; i++) {
    sprintf(mqtt_message, "{\"pixelid\": %d, \"R\": %d, \"G\": %d, \"B\": %d, \"W\": 0}", i, R, G, B);
    Serial.println(mqtt_message);

    if (client.publish(mqtt_topic_color, mqtt_message)) {
      Serial.println("Color message published");
    } else {
      Serial.println("Failed to publish color message");
    }
    delay(500);
  }
}

// send brightness data to MQTT
void sendBrightness(int brightness) {
  char mqtt_message[100];
  sprintf(mqtt_message, "{\"brightness\": %d}", brightness);
  Serial.println(mqtt_message);

  if (client.publish(mqtt_topic_brightness, mqtt_message)) {
    Serial.println("Brightness message published");
  } else {
    Serial.println("Failed to publish brightness message");
  }
}

void startWifi() {
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("No networks found");
  } else {
    Serial.print("Trying to connect to: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(600);
      Serial.print(".");
    }
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

void reconnectMQTT() {
  if (WiFi.status() != WL_CONNECTED) {
    startWifi();
  }

  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "LuminaSelector";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Connected to MQTT");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
