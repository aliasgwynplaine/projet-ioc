#include <WiFi.h>
#include <PubSubClient.h>

//convert int values read from lightsensor and led to int 
char luxVal[20];
char ledVal[20];


#define PHOTOLIGHT_PIN 36
const int btnPin = 23;
const int ledPin = LED_BUILTIN;
int state = 0;
int prev = 0;
bool on = false;

enum lightstate {DARK, DIM, LIGHT, BRIGHT, VERYBRIGHT};
lightstate l = DARK;
lightstate prevlightstate;

/* MQTT STUFF */
const char *topiclux = "luxlevel";
const char *topicled = "ledstate";
const char *myid  = "esp32-leon";
const char *mqtt_username = "";
const char *mqtt_password = "";
const char *mqtt_broker = "192.168.83.207"; // THIS MAY CHANGE 
const int mqtt_port = 1883;
WiFiClient wificlient;
PubSubClient mqttclient(wificlient);

void senddatafux(char *topic, byte *payload, unsigned int length) {
  Serial.println("Sending data...");
}

void setuplux() {
  Serial.begin(9600);
  pinMode(PHOTOLIGHT_PIN, INPUT);
  pinMode(ledPin, OUTPUT);

  /* wifi connection */
  WiFi.begin("lailux", "vzno3113"); // lol in plaintext

  while(WiFi.status()!= WL_CONNECTED){
    delay(500);
    Serial.println("connecting...");
  }

  /* MQTT Setting... */
  
  Serial.println("Attempting to connect the mqtt server...");
  mqttclient.setServer(mqtt_broker, mqtt_port);
  mqttclient.setCallback(senddatafux);

  while (!mqttclient.connected()) {
    if (mqttclient.connect(myid, NULL, NULL)) {
      Serial.println("Yippee!");
    } else {
      Serial.println("trying...");
      delay(500);
    }
  }

  mqttclient.publish(topiclux, "hello, friend lux");
  delay(2000);
  mqttclient.publish(topiclux, "starting wave of messages lux....");

}

void setupled() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(btnPin, INPUT_PULLUP);  // 0x05
  pinMode(ledPin, OUTPUT);        // 0x03

  state = digitalRead(btnPin);

  /* wifi connection */
  WiFi.begin("lailux", "vzno3113"); // lol in plaintext

  while(WiFi.status()!= WL_CONNECTED){
    delay(500);
    Serial.println("connecting...");
  }

  /* MQTT Setting... */
  
  Serial.println("Attempting to connect the mqtt server...");
  mqttclient.setServer(mqtt_broker, mqtt_port);
  mqttclient.setCallback(senddatafux);

  while (!mqttclient.connected()) {
    if (mqttclient.connect(myid, NULL, NULL)) {
      Serial.println("Yippee!");
    } else {
      Serial.println("trying...");
      delay(500);
    }
  }

  mqttclient.publish(topicled, "hello, friend led");
  delay(2000);
  mqttclient.publish(topicled, "starting wave of messages led....");

}

void loopled() {
  // put your main code here, to run repeatedly:
  
  prev = state;
  
  state = digitalRead(btnPin);

  //Serial.println(state);
  if (prev == 1 && state == 0){
    on = !on;
    digitalWrite(ledPin, on);
    itoa(on, ledVal, 10);
    Serial.println(ledVal);
    mqttclient.publish(topicled, ledVal);
  }



  
}

void looplux() {
  int analogValue = analogRead(PHOTOLIGHT_PIN);

  Serial.print("Analog Value = ");
  Serial.print(analogValue);   // the raw analog reading

  prevlightstate = l;

  // We'll have a few threshholds, qualitatively determined
  if (analogValue < 40) {
    l = VERYBRIGHT;
  } else if (analogValue < 800) {
    l = BRIGHT;
  } else if (analogValue < 2000) {
    l = LIGHT;
  } else if (analogValue < 3200) {
    l = DIM;
  } else {
    l = DARK;
  }

  if (l != prevlightstate){
    itoa(analogValue, luxVal, 10);
    mqttclient.publish(topiclux, luxVal);
    Serial.println(luxVal);
  }

}

void setup(){
  setuplux();
  setupled();
}

void loop(){
  looplux();
  loopled();
}
