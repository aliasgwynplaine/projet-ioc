#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "WiFi.h"
#include "PubSubClient.h"

const int btnPin = 23;  
const int buzzerPin = 17;
const int ledPin = LED_BUILTIN;
int state = 0;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET    16 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const String s = String("yippee!!");
int musica[] = {
13, 293, 0, 293, 586, 440, 413, 386, 0, 346, 0, 293, 0, 346, 386, 0, 266, 0, 266, 586, 440, 413, 386, 786, 413, 346, 0, 293, 346, 0, 386, 0, 253, 0, 586, 240, 586, 440, 413, 386, 346, 293, 0, 346, 0, 386, 0, 586, 440, 413, 386, 346, 293, 0, 346, 386, 0, 293, 0, 293, 586, 440, 0, 146, 440, 413, 386, 413, 346, 0, 293, 346, 386, 0, 266, 586, 266, 586, 440, 0, 120, 413, 386, 413, 386, 346, 293, 0, 346, 386, 120, 0, 586, 240, 586, 440, 0, 120, 240, 133, 413, 386, 346, 293, 346, 386, 0, 466, 0, 586, 440, 120, 453, 413, 386, 120, 413, 346, 0, 293, 346, 0, 386, 440, 80, 146, 80, 146, 80, 146, 53, 146, 80, 146, 80, 146, 66, 133, 160, 66, 133, 66, 133, 66, 133, 66, 146, 66, 133, 66, 133, 53, 1173, 120, 66, 120, 53, 120, 66, 200, 120, 53, 120, 66, 186, 53, 120, 66, 120, 53, 120, 1173, 120, 80, 106, 53, 133, 66, 133, 93, 66, 133, 66, 93, 66, 146, 293, 146, 53, 146, 293, 146, 80, 146, 53, 146, 80, 53, 146, 66, 293, 146, 66, 1173, 66, 133, 80, 133, 66, 133, 66, 266, 66, 133, 66, 133, 80, 53, 120, 66, 120, 66, 186, 53, 120, 66, 133, 53, 120, 80, 53, 120, 66, 120, 80, 53, 1173, 120, 80, 106, 53, 120, 66, 133, 53, 133, 66, 893, 80, 146, 733, 146, 173, 146, 80, 146, 586, 146, 80, 133, 66, 133, 66, 133, 66, 133, 66, 786, 66, 133, 66, 120, 66, 693, 133, 53, 133, 53, 120, 66, 120, 66, 120, 53, 120, 880, 120, 80, 120, 1173, 120, 880, 120, 53, 133, 1053, 66, 133, 66, 1773
};

int duracion[] = {
100, 100, 100, 100, 400, 500, 299, 299, 100, 200, 100, 100, 100, 100, 100, 100, 100, 100, 100, 299, 500, 400, 100, 100, 100, 299, 100, 100, 100, 100, 100, 100, 100, 200, 100, 100, 100, 500, 299, 400, 299, 100, 100, 100, 100, 100, 299, 400, 500, 299, 299, 400, 100, 100, 100, 100, 100, 100, 100, 100, 299, 200, 100, 100, 100, 400, 200, 100, 299, 100, 100, 200, 100, 100, 299, 100, 100, 100, 100, 200, 200, 299, 200, 100, 100, 299, 100, 100, 100, 200, 100, 200, 200, 100, 100, 100, 100, 100, 100, 100, 299, 299, 400, 100, 200, 100, 100, 100, 200, 299, 200, 200, 100, 400, 100, 100, 100, 299, 100, 100, 100, 100, 100, 100, 100, 700, 100, 600, 100, 200, 100, 100, 100, 200, 100, 200, 200, 100, 100, 100, 200, 100, 100, 100, 500, 100, 299, 299, 100, 200, 200, 100, 100, 100, 100, 299, 299, 200, 100, 100, 100, 200, 100, 100, 100, 100, 200, 100, 299, 100, 100, 100, 299, 100, 400, 200, 200, 100, 299, 100, 100, 100, 200, 100, 299, 200, 100, 200, 100, 100, 100, 299, 100, 100, 100, 299, 100, 100, 200, 100, 100, 200, 100, 200, 100, 200, 400, 500, 100, 200, 100, 100, 200, 100, 100, 200, 100, 100, 200, 100, 200, 100, 100, 100, 200, 200, 200, 100, 200, 200, 100, 100, 200, 200, 100, 100, 200, 299, 100, 400, 100, 200, 100, 400, 100, 100, 400, 100, 100, 800, 100, 299, 100, 100, 100, 200, 700, 100, 200, 500, 100, 100, 100, 600, 100, 200, 299, 100, 200, 100, 200, 299, 100, 100, 100, 100, 100, 100, 200, 200, 100, 200, 200, 200, 100, 299, 200, 100, 200, 100, 400, 100, 200, 200, 100, 100, 100, 299, 200, 100
};

void drawchar(char c, int x){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(x,10);
  display.cp437(true);

  display.write(c);
  display.display();
  delay(200);
}


void setup() {
  // put your setup code here, to run once:
  Wire.begin(4,15); //setup pins for esp32
  Serial.begin(9600);
  /*
  WiFi.begin("lailux", "vzno3113");
  while(WiFi.status()!= WL_CONNECTED){
    delay(500);
    Serial.println("connecting...");
  }
  */

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for this specfic 128x64 led 
    Serial.println("SSD1306 allocation failed");
    for(;;); // Don't proceed, loop forever
  }

  Serial.println("yippee!! screen connected !!11!1!11!");
  delay(500);

  //show initial display buffer
  display.display();
  delay(2000);

  display.clearDisplay();

  pinMode(btnPin, INPUT_PULLUP);  // 0x05
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin,OUTPUT);


}

void loop() {
  // put your main code here, to run repeatedly:
  state = digitalRead(btnPin);
  //Serial.println(state);
  int x = 10;
  digitalWrite(ledPin, !state);
  for(int i = 0; i<s.length(); i++){
    drawchar(s[i], x);
    x+=5;
  }

  
  for(int i =0; i<200; i++){
    tone(buzzerPin,  musica[i], duracion[i]);
    delay(duracion[i]*0.3);
  }
  
}
