#include <WiFi.h>
#include <PubSubClient.h>

#define pirinput 14 // pir input pin
#define echoPin 26 // attach pin G26 to Echo of HC-SR04
#define trigPin 25 // attach pin G25 to Trig of HC-SR04
#define led 2       // built in led for movement feedback

// defines variables
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement
const int threshold = 500; // distance threshold in mm-s
boolean occupied;
boolean state;
boolean result_mqtt;
boolean result_mqtt2;
const char* ssid     = "FASTWEB-EC4E89_EXT";
const char* password = "729PFRG4ZR";
const char* ip_mqtt_server = "192.168.1.141";

PubSubClient client_mqtt;
WiFiClient client_wifi;

// function to connect to the wifi
void connect() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connection attempt");
    delay(500);
  }
  
  delay(5000);
  Serial.println("WiFi connected");
  
  // Start the server
  Serial.println(WiFi.localIP());
  
}

// publishing data to the mqtt server
  boolean publishData(int channel, float value) {
  bool result = false;
  boolean connected = client_mqtt.connected();

  // connecting to the mqtt server before sending data
  if (!connected) 
    connected = client_mqtt.connect("MYesp32");
  if (connected) {

      // converting float value to string
      String message = String(value);
      const char* payload = message.c_str();

      // deciding which topic to publish depending on the function parameter
      if (channel == 0) 
        result=client_mqtt.publish("parking/Garage1/Slot1/motion",payload);
      else if (channel == 1)
        result=client_mqtt.publish("parking/Garage1/Slot1/distance",payload);
      else if (channel == 2)
        result = client_mqtt.publish("parking/Garage1/Slot1/occ",payload);
      else if (channel == 3)
        result=client_mqtt.publish("parking/Garage1/Slot2/motion",payload);
      else if (channel == 4)
        result = client_mqtt.publish("parking/Garage1/Slot2/distance",payload);
      else
        result = client_mqtt.publish("parking/Garage1/Slot2/occ",payload);

      // looping the client to be able to send multiple msgs
      // returning if msg transfer was successful
      client_mqtt.loop();
      return result;
  } 
  else
    return(false);  
}

void setup() {
  // put your setup code here, to run once:
  //pinMode(pirinput, INPUT);
  Serial.begin(115200);

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  pinMode(led, OUTPUT); // led output to see motion

  delay(10);
  
  // connecting and setting up clients and server instances
  connect();
  client_mqtt.setClient(client_wifi);
  client_mqtt.setServer(ip_mqtt_server, 1883);
  client_mqtt.setBufferSize(400);
  result_mqtt = false;  
}

void loop() {
  // put your main code here, to run repeatedly:
  int motion;
  
  motion = digitalRead(pirinput);   // read sensor value
  if (motion == HIGH) {           // check if the sensor is HIGH
    delay(250);
    
    // if we detect motion and previously there were not any, we detected a motion (change)
    if (state == LOW) {
      Serial.println("Motion detected!"); 
      state = HIGH;       // update variable state to HIGH
      digitalWrite(led, state);

      // publishing motion to 2 different parking slots.
      // second one is the negated values of the first one
      result_mqtt = publishData(0, 1.0);
      result_mqtt2 = publishData(3, 0.0);

      //diagnostic msg to the serial monitor
      if (result_mqtt and result_mqtt2) 
        Serial.println("[LOG] Motion published on the MQTT server");
      else
        Serial.println("[ERROR] MQTT connection failed");
    }
    else{
      //if same state as prev, then no motion
      result_mqtt = publishData(0, 0.0);
      result_mqtt2 = publishData(3, 1.0);

      // diagnostic
      if (result_mqtt and result_mqtt2) 
        Serial.println("[LOG] Motion published on the MQTT server");
      else
        Serial.println("[ERROR] MQTT connection failed");
    }
    
  } 
  else {
      //checking if motion is stopped
      delay(250);     
      if (state == HIGH){
        Serial.println("Motion stopped!");
        state = LOW;       // update variable state to LOW
        digitalWrite(led, state);

        // opposite value for other topic
        result_mqtt = publishData(0, 0.0);
        result_mqtt2 = publishData(3, 1.0);

        // diagnostic msg
        if (result_mqtt and result_mqtt2) 
          Serial.println("[LOG] Motion published on the MQTT server");
        else
          Serial.println("[ERROR] MQTT connection failed");
    }
    else{
      // the motion has not stopped in this case
      result_mqtt = publishData(0, 1.0);
      result_mqtt2 = publishData(3, 0.0);

      // diagnostic msg
      if (result_mqtt and result_mqtt2) 
        Serial.println("[LOG] Motion published on the MQTT server");
      else
        Serial.println("[ERROR] MQTT connection failed");
    }
  }

  // Distance meas
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculating the distance in mm-s
  distance = duration * 0.34 / 2.0; // Speed of sound wave divided by 2 (go and back)

  // publishing half distance every time for the other topic (slot)
  result_mqtt = publishData(1, distance);
  result_mqtt2 = publishData(4, distance / 2.0);

  // diagnostic msg
  if (result_mqtt and result_mqtt2) 
    Serial.println("[LOG] Distance published on the MQTT server");
  else
    Serial.println("[ERROR] MQTT connection failed");

  // checking occupance condition
  if(distance < threshold)
    occupied = 1.0;
  else
    occupied = 0.0;

  //publishing
  result_mqtt = publishData(2, occupied);

  // if distance is smaller than threshold that means that there is a vehicle
  if(distance / 2.0 < threshold)
    occupied = 1.0;
  else
    occupied = 0.0;
    
  result_mqtt2 = publishData(5, occupied);
  
  if (result_mqtt and result_mqtt2) 
    Serial.println("[LOG] Occupance published on the MQTT server");
  else
    Serial.println("[ERROR] MQTT connection failed");

  // waiting 30s
  delay(1000*30);
}
