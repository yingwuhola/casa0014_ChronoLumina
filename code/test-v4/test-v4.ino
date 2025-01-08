#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <utility/wifi_drv.h>   // library to drive RGB LED on the MKR1010
#include "arduino_secrets.h"

#define hallPin A6   // hall sensor connected to A6
#define lightPin A0  // light sensor connected to A0
const int modeButtonPin = 2; // pin for the mode button

// WiFi and MQTT settings
const char* ssid          = SECRET_SSID;
const char* password      = SECRET_PASS;
const char* mqtt_username = SECRET_MQTTUSER;
const char* mqtt_password = SECRET_MQTTPASS;
const char* mqtt_server   = "mqtt.cetools.org";
const int mqtt_port       = 1884;

WiFiServer server(80);
WiFiClient wificlient;
WiFiClient mkrClient;
PubSubClient client(mkrClient);

// set MQTT topics and message
char mqtt_topic_color[] = "student/CASA0014/light/47/pixel/";
char mqtt_topic_brightness[] = "student/CASA0014/light/47/brightness/";
char mqtt_message[100];

int brightness = 90; // initialize the brightness value
int mode = 0; // 0 for default mode, 1 for reading mode

void setup() {
  Serial.begin(115200);   // Start the serial monitor to show output
  delay(1000);

  WiFi.setHostname("Lumina ucfnwut");
  startWifi();
  client.setServer(mqtt_server, mqtt_port);
  Serial.println("Setup complete");

  pinMode(modeButtonPin, INPUT_PULLUP); // Set the mode button pin as an input with pull-up
}

void loop() {
  // make sure the arduino is connected to the MQTT broker & wifi
  // otherwise we will not receive any messages & connect to MQTT
  if (!client.connected()) {
    reconnectMQTT();
  }
  if (WiFi.status() != WL_CONNECTED) {
    startWifi();
  }

  // check for messages from the broker and ensuring that any outgoing messages are sent.
  client.loop(); 

  // Check if the mode button is pressed
  if (digitalRead(modeButtonPin) == LOW) {
    // Toggle the mode
    mode = 1 - mode;
    // Wait for the button to be released
    delay(200);
    // Wait until the button is not pressed to avoid bouncing
    while (digitalRead(modeButtonPin) == LOW) {
      delay(10);
    }
  }

  int hallValue = analogRead(hallPin);  //read the analog value from the hall sensor
  setColor(hallValue);    //set the color

  int lightValue = analogRead(lightPin);  //read the analog value from the light sensor
  setBrightness(lightValue);  //adjust brightness based on light sensor

  delay(500);
}

// set the color
void setColor(int hallValue) {
  int R = 0, G = 0, B = 0;
  if (hallValue < 350) {
    R = 255; G = 200; B = 70;  // Red
  } else if (hallValue >= 350 && hallValue <= 700) {
    R = 255; G = 223; B = 128;  // Yellow
  } else {
    R = 200; G = 220; B = 230;  // Green
  }

  Serial.print("Hall Sensor Value: ");
  Serial.println(hallValue);

  sendColor(R, G, B);  // send RGB color data to MQTT
}

// send RGB color data to MQTT
void sendColor(int R, int G, int B) {
  for (int i = 0; i < 12; i++) {   // Send color data for each pixel
    sprintf(mqtt_message, "{\"pixelid\": %d, \"R\": %d, \"G\": %d, \"B\": %d, \"W\": 0}", i, R, G, B);
    Serial.println(mqtt_message);

    if (client.publish(mqtt_topic_color, mqtt_message)) {
      Serial.println("Color message published");
    } else {
      Serial.println("Failed to publish color message");
    }
  }
}

//adjust brightness based on light sensor
void setBrightness(int lightValue) {
  int brightnessLevel;
  if (mode == 0) { // Default mode
    if (lightValue <= 400) {
      brightnessLevel = 50;
    } else if (lightValue > 400 && lightValue <= 800) {
      brightnessLevel = 90;
    } else {
      brightnessLevel = 119;
    }
  } else { // Reading mode
    if (lightValue <= 400) {
      brightnessLevel = 119;
    } else if (lightValue > 400 && lightValue <= 800) {
      brightnessLevel = 90;
    } else {
      brightnessLevel = 50;
    }
  }

  brightness = brightnessLevel; // Update the global brightness variable

  sendBrightness(brightness);  // send brightness data to MQTT

  Serial.print("Light Sensor Value: ");
  Serial.print(lightValue);
  Serial.print(" -> Brightness Level: ");
  Serial.println(brightness);
}

// send brightness data to MQTT
void sendBrightness(int brightness) {
  sprintf(mqtt_message, "{\"brightness\": %d}", brightness);
  Serial.println(mqtt_message);

  if (client.publish(mqtt_topic_brightness, mqtt_message)) {
    Serial.println("Brightness message published");
  } else {
    Serial.println("Failed to publish brightness message");
  }
}

// WiFi connection
void startWifi(){
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);  
  }
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  int n = WiFi.scanNetworks();
  Serial.println("Scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    Serial.print("Trying to connect to: ");
    Serial.println(ssid);
    
    for (int i = 0; i < n; ++i){
      String availablessid = WiFi.SSID(i);
      if (availablessid.equals(ssid)) {
        Serial.print("Connecting to ");
        Serial.println(ssid);
        WiFi.begin(ssid, password);
        
        while (WiFi.status() != WL_CONNECTED) {
          delay(600);
          Serial.print(".");
        }

        if (WiFi.status() == WL_CONNECTED) {
          Serial.println("Connected to " + String(ssid));
          break; 
        } else {
          Serial.println("Failed to connect to " + String(ssid));
        }
      } else {
        Serial.print(availablessid);
        Serial.println(" - this network is not in my list");
      }

    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

}

// reconnect to MQTT broker
void reconnectMQTT() {
  while (!client.connected()) { 
    Serial.print("Attempting MQTT connection...");
    String clientId = "LuminaSelector";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, int length) {
  // Handle incoming messages
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

}