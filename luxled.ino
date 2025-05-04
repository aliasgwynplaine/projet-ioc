#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "musicinfo.h"

//music stuff
int index_musica = 0;
int note_index = 0;
bool play = false;

//screen stuff
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET    16 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
const String printed_string = String("keep yourself safe!11111!!!!");

//pins declarations
#define PHOTOLIGHT_PIN 36
#define BUZZER_PIN 17
#define BUTTON_PIN 23

//timer for non blocking delays in music
unsigned long time_now = 0;

//photores val
int ledstate = 0;
int prevledstate = 0;
bool ledon = false;

//converted values to string
char luxVal[(20)];
char ledVal[2];

//photores val
enum lightstate {DARK, DIM, LIGHT, BRIGHT, VERYBRIGHT};
lightstate l = DARK;
lightstate prevlightstate;

/* MQTT STUFF */
const char *topiclux = "luxlevel";
const char *topicled = "ledstate";
const char *myid  = "esp32-leon";
const char *mqtt_username = "";
const char *mqtt_password = "";
const char *mqtt_broker = "192.168.7.207"; // THIS MAY CHANGE 
const int mqtt_port = 1883;
WiFiClient wificlient;
PubSubClient mqttclient(wificlient);

void reconnect() {
  // Loop until we're reconnected
  while (!mqttclient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttclient.connect(myid)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      // resubscribe
      mqttclient.subscribe("musica");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttclient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);  
    }
  }
}


//--------- définition de la fonction waitFor
#define MAX_WAIT_FOR_TIMER 4
unsigned int waitFor(int timer, unsigned long period){
  static unsigned long waitForTimer[MAX_WAIT_FOR_TIMER];  // il y a autant de timers que de tâches périodiques
  unsigned long newTime = micros() / period;              // numéro de la période modulo 2^32 
  int delta = newTime - waitForTimer[timer];              // delta entre la période courante et celle enregistrée
  if ( delta < 0 ) delta = 1 + newTime;                   // en cas de dépassement du nombre de périodes possibles sur 2^32 
  if ( delta ) waitForTimer[timer] = newTime;             // enregistrement du nouveau numéro de période
  return delta;
}

//function to print a character on the screen
void drawchar(char c, int x){
  display.setCursor(x,10);
  display.write(c);
}


//callback function to receive information from server
void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  if((payload[0] - '0') == 4){
    play = false;
  } else {
    index_musica = payload[0] - '0';
    play = true;
  }
  note_index = 0;

}

//--------- définition de la tache Lux

struct lux_s {
  int timer;            // numéro du timer pour cette tâche utilisé par WaitFor
  unsigned long period; // periode de clignotement
  int pin;              // numéro de la broche sur laquelle est la LED
}; 

//init photoresistance
void setuplux(struct lux_s * lux, int timer, unsigned long period, byte pin) {
  lux->timer = timer;
  lux->period = period;
  lux->pin = pin;
  pinMode(lux->pin, INPUT);
  
}

//reads photores value, maps it, and sends it to the server when it changes
void looplux(struct lux_s * lux) {
  if (!(waitFor(lux->timer,lux->period))) return; 

  int analogValue = analogRead(lux->pin);

  prevlightstate = l;
  // We'll have a few threshholds, qualitatively determined
  if (analogValue < 40) {
    l = DARK;
  } else if (analogValue < 800) {
    l = DIM;
  } else if (analogValue < 2000) {
    l = LIGHT;
  } else if (analogValue < 3200) {
    l = BRIGHT;
  } else {
    l = VERYBRIGHT;
  }

  if (l != prevlightstate){
    itoa(analogValue, luxVal, 10);
    mqttclient.publish(topiclux, luxVal);
    Serial.println(luxVal);
  }

}


//--------- definition de la tache oled
struct oled_s {
  int timer;              // numéro du timer pour cette tâche utilisé par WaitFor
  unsigned long period;   // periode de clignotement                                               
}; 

//init screen
void setupscreen(struct oled_s * screen, int timer, unsigned long period){
  Wire.begin(4,15); //setup pins for esp32

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for this specfic 128x64 led 
    Serial.println("SSD1306 allocation failed");
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();

  screen->timer = timer;
  screen->period = period;

  display.setTextSize(1);
  display.setTextColor(WHITE);

}


//prints currently playing song name on screen
void loopscreen(struct oled_s * screen){
  if (!(waitFor(screen->timer,screen->period))) return;         // sort s'il y a moins d'une période écoulée

  display.clearDisplay();
  int x = 10;
  for(int i = 0; i<song_names[index_musica].length(); i++){
    drawchar(song_names[index_musica][i], x);
    x+=5;
  }
  display.display();

}

//--------- définition de la tache Led

struct ledbutton_s {
  int timer;           // numéro du timer pour cette tâche utilisé par WaitFor
  int pinled;          // numéro de la broche sur laquelle est la LED
  int pinbutton;       // numéro de la broche sur laquelle est le bouton
  int period;          // periode de clignotement
  int etat;            // etat interne de la led
}; 

//init led and button
void setupledbutton(struct ledbutton_s * lb, int timer, int period, byte pinled, byte pinbutton) {
  lb->timer = timer;
  lb->pinled = pinled;
  lb->pinbutton = pinbutton;
  lb->period = period;
  lb->etat = 0;


  pinMode(lb->pinbutton, INPUT_PULLUP);  // 0x05
  pinMode(lb->pinled, OUTPUT);        // 0x03

  ledstate = digitalRead(lb->pinbutton);

  mqttclient.publish(topicled, "hello, friend led");
  mqttclient.publish(topicled, "starting wave of messages led....");

}

//reads button val, changes led val accordingly and sends it to server
void loopledbutton(struct ledbutton_s *lb) {
  
  if (!waitFor(lb->timer, lb->period)) return;
  // put your main code here, to run repeatedly:
  
  prevledstate = lb->etat;
  
  lb->etat= digitalRead(lb->pinbutton);

  //Serial.println(state);
  if (prevledstate == 1 && lb->etat == 0){
    ledon = !ledon;
    Serial.print("-------button pressed ");
    digitalWrite(lb->pinled, ledon);
    itoa(ledon, ledVal, 2);
    Serial.println(ledVal);
    mqttclient.publish(topicled, ledVal);
  }

}

//--------- définition de la tache buzzer

struct buzzer_s {
  int timer;                // numéro de timer utilisé par WaitFor
  unsigned long period;     // periode d'affichage
  int pin;                  // numéro de la broche sur laquelle est la LED
}; 

//init buzzer
void setupmusic(struct buzzer_s * buz, int timer, unsigned long period, byte pin){
  buz->timer = timer;
  buz->period = period;
  buz->pin = pin;
  pinMode(buz->pin, OUTPUT);
}


//plays the chosen song by the server on the buzzer 
void loopmusic(struct buzzer_s * buz){

  if (!waitFor(buz->timer, buz->period)) return;
  
  if (!play) {
    noTone(buz->pin);
    return;
  }
  
  int size = sizeof(durations[index_musica]) / sizeof(int);

  float duration = durations[index_musica][note_index]*duration_speed[index_musica];

  //to distinguish the notes, set a minimum time between them.
  int delaytime = duration;

  if(!time_now){
    time_now = millis();
    tone(buz->pin, melody[index_musica][note_index], duration);
  }

  if(millis() > time_now + 10){
    noTone(buz->pin);
    time_now = 0;
    note_index = (note_index+1)%size;
  }
}


//--------- Déclaration des tâches

struct ledbutton_s lb;
struct oled_s screen;
struct lux_s lum;
struct buzzer_s buz;


void setup(){
  //------------------ SETUP WIFI AND MQTT 

  /* wifi connection */
  WiFi.begin("lailux", "vzno3113"); // lol in plaintext

  while(WiFi.status()!= WL_CONNECTED){
    Serial.println("connecting...");
  }
  
  /* MQTT Setting... */
  /*
  Serial.println("Attempting to connect the mqtt server...");
  mqttclient.setServer(mqtt_broker, mqtt_port);
  mqttclient.setCallback(callback);

  while (!mqttclient.connected()) {
    if (mqttclient.connect(myid, NULL, NULL)) {
      Serial.println("Yippee!");
    } else {
      Serial.println("trying...");
    }
  }

  mqttclient.subscribe("musique");
  */

  mqttclient.setServer(mqtt_broker, mqtt_port);
  mqttclient.setCallback(callback);

  Serial.begin(9600);


  setuplux(&lum, 0, 1000000 , PHOTOLIGHT_PIN);
  setupledbutton(&lb, 1, 100000, LED_BUILTIN, BUTTON_PIN);
  setupscreen(&screen, 2, 1000000);
  setupmusic(&buz, 3, 100000, BUZZER_PIN);

  delay(1500);
}

void loop(){
  if (!mqttclient.connected()) {
    reconnect();
  }
  mqttclient.loop();
  looplux(&lum);
  loopledbutton(&lb);
  loopscreen(&screen);
  loopmusic(&buz);
}