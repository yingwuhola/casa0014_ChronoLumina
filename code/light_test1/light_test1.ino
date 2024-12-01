#include <WiFiNINA.h>   
#include <PubSubClient.h>
#include <utility/wifi_drv.h>   // library to drive to RGB LED on the MKR1010
#include "arduino_secrets.h" 

#define lightPin A0 //light sensor connected to A0
int brightness = 90; //initialize the brightness value

//WiFi and MQTT settings
const char* ssid          = SECRET_SSID;
const char* password      = SECRET_PASS;
const char* mqtt_username = SECRET_MQTTUSER;
const char* mqtt_password = SECRET_MQTTPASS;
const char* mqtt_server = "mqtt.cetools.org";
const int mqtt_port = 1884;
int status = WL_IDLE_STATUS;     // the Wifi radio's status

WiFiServer server(80);
WiFiClient wificlient;
WiFiClient mkrClient;
PubSubClient client(mkrClient);


// edit this for the light you are connecting to
char mqtt_topic_demo[] = "student/CASA0014/light/45/brightness/";
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

  
  int lightValue = analogRead(lightPin); //read the analog value from the light sensor
  sendBrightness(lightValue); //adjust brightness


  sendmqtt();
  Serial.println("sent a message");
  delay(500);
  
}


//adjust brightness based on light sensor
void sendBrightness(int lightValue){
  //set brightness
  if (lightValue <= 400){
    brightness=119;       //high brightness in dim light
  } else if (lightValue > 400 && lightValue <=800){
    brightness = 90;      //moderate brightness
  } else {
    brightness = 50;      //low brightness in bright light
  }

Serial.print("Light Sensor Value: ");
Serial.print(lightValue);
Serial.print(" -> Brightness Level: ");
Serial.println(brightness);


//send brightness message to MQTT
//sprintf(mqtt_topic_demo, "student/CASA0014/light/45/brightness/");
//sprintf(mqtt_message, "{\"brightness\": %d}", brightness);

}


void sendmqtt(){

  // send a message to update the light
  char mqtt_message[100];

  for(int i=0; i < 12; i++){
  sprintf(mqtt_message, "{\"pixelid\": %d, \"R\": 255, \"G\": 0, \"B\": 0, \"W\": 0}", i);
  Serial.println(mqtt_topic_demo);
  Serial.println(mqtt_message);
  

  if (client.publish(mqtt_topic_demo, mqtt_message)) {
    Serial.println("Message published");
  } else {
    Serial.println("Failed to publish message");
  }
  //delay(500);
}
}

//wifi
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

//mqtt
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