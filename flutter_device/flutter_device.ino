#include <Servo.h>  
#include <WiFiNINA.h>
#include <PubSubClient.h>
#define RAINRATE1 A0
#define RAINRATE2 A1
#define RAINRATE3 A2
#define SERVO1 4
#define SERVO2 5
#define SERVO3 6 
int rainrate1=0;
int rainrate2=0;
int rainrate3=0;
Servo myservo1;
Servo myservo2;
Servo myservo3;

#define SECRET_SSID "CE-Hub-Student"
#define SECRET_PASS "casa-ce-gagarin-public-service"
#define SECRET_MQTTUSER "student"
#define SECRET_MQTTPASS "ce2021-mqtt-forget-whale"

const char* ssid     = SECRET_SSID;
const char* password = SECRET_PASS;
const char* mqttuser = SECRET_MQTTUSER;
const char* mqttpass = SECRET_MQTTPASS;


const char* mqtt_server = "mqtt.cetools.org";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
  Serial.begin(9600);
  pinMode(RAINRATE1,INPUT);
  pinMode(RAINRATE2,INPUT);
  pinMode(RAINRATE3,INPUT);
  startWifi();
  // start MQTT server
  client.setServer(mqtt_server, 1884);
  client.setCallback(callback);
  myservo1.attach(SERVO1);
  myservo2.attach(SERVO2);
  myservo3.attach(SERVO3);
}

void loop() {

  delay(2000);
  rainrate1 = map(analogRead(RAINRATE1), 0, 1023, 235, 0);
  rainrate2 = map(analogRead(RAINRATE2), 0, 1023, 235, 0);
  rainrate3 = map(analogRead(RAINRATE3), 0, 1023, 235, 0);
  Serial.println("rain = ");
  Serial.println(rainrate1);
  Serial.println(rainrate2);
  Serial.println(rainrate3);
  sendMQTT();
  client.loop();

}



void startWifi() {
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // check to see if connected and wait until you are
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void sendMQTT() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  snprintf (msg, 50, "%.0i", rainrate1);
  Serial.print("Publish message for t: ");
  Serial.println(msg);
  client.publish("student/CASA0014/plant/ucfnaaf/rainrate1", msg);


  snprintf (msg, 50, "%.0i", rainrate2);
  Serial.print("Publish message for h: ");
  Serial.println(msg);
  client.publish("student/CASA0014/plant/ucfnaaf/rainrate2", msg);

  //Moisture = analogRead(soilPin);   // moisture read by readMoisture function
  snprintf (msg, 50, "%.0i", rainrate3);
  Serial.print("Publish message for m: ");
  Serial.println(msg);
  client.publish("student/CASA0014/plant/ucfnaaf/rainrate3", msg);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");


   if (strcmp(topic, "student/CASA0014/plant/ucfnaaf/servo2") == 0) {
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  float servo22= atof(payload);
  myservo2.write(servo22*18);
  Serial.println("for the servo2");
  }else if (strcmp(topic, "student/CASA0014/plant/ucfnaaf/servo3") == 0) {
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  float servo33= atof(payload);
  myservo3.write(servo33*18);
  Serial.println("for the servo3");
  }else if (strcmp(topic, "student/CASA0014/plant/ucfnaaf/servo1") == 0) {
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  float servo11= atof(payload);
  myservo1.write(servo11*18);
  Serial.println("for the servo1");
  }

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '2') {
  // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because it is active low on the ESP-01)
  } else if((char)payload[0] == '1') {

  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect with clientID, username and password
    if (client.connect(clientId.c_str(), mqttuser, mqttpass)) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe("student/CASA0014/plant/ucfnaaf/inTopic");
      client.subscribe("student/CASA0014/plant/ucfnaaf/servo1");
      client.subscribe("student/CASA0014/plant/ucfnaaf/servo2");
      client.subscribe("student/CASA0014/plant/ucfnaaf/servo3");
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}