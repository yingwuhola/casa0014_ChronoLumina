#include <WiFiNINA.h>   
#include <PubSubClient.h>
#include <utility/wifi_drv.h>   // library to drive to RGB LED on the MKR1010
#include "arduino_secrets.h" 

#define hallPin A1   //hall sensor connected to A1

//WiFi and MQTT settings
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

// MQTT topic for the light
char mqtt_topic_demo[] = "student/CASA0014/light/45/pixel/";
//set MQTT topic and messgae
//char mqtt_topic_demo[100];
char mqtt_message[100];


void setup() {
  Serial.begin(115200);   // Start the serial monitor to show output
  delay(1000);

  WiFi.setHostname("Lumina ucfnwut");
  startWifi();
  client.setServer(mqtt_server, mqtt_port);
  Serial.println("setup complete");
}

void loop() {
  // make sure the arduino is connected to the MQTT broker & wifi
  // otherwise we will not receive any messagesc & connect to MQTT
  if (!client.connected()) {
    reconnectMQTT();
  }
  if (WiFi.status() != WL_CONNECTED){
    startWifi();
  }

  // check for messages from the broker and ensuring that any outgoing messages are sent.
  client.loop();


  int hallValue = analogRead(hallPin); //read the analog value from the hall snesor
  //Serial.print("Hall Sensor Value: ");
  //Serial.println(hallValue);
  
  //set the color
  int R = 0, G = 0, B = 0;
  if (hallValue < 350) {
    R = 255; G = 0; B = 0;  // red color
  } else if (hallValue >= 350 && hallValue <= 550) {
    R = 255; G = 255; B = 0;  //yellow color
  } else {
    R = 0; G = 255; B = 0;  //green color
  }

  sendColor(R, G, B);
  //Serial.println("sent a message");
  delay(500);

}

//send RGB color to MQTT
void sendColor(int R, int G, int B) {
  char mqtt_message[100];

  for (int i = 0; i < 12; i++) {  //send color to each pixel
    sprintf(mqtt_message, "{\"pixelid\": %d, \"R\": %d, \"G\": %d, \"B\": %d, \"W\": 0}", i, R, G, B);
    Serial.println(mqtt_topic_demo);
    Serial.println(mqtt_message);


    if (client.publish(mqtt_topic_demo, mqtt_message)) {
      Serial.println("Message published");
    } else {
      Serial.println("Failed to publish message");
    }
    //delay(500); // delay for each pixel
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
