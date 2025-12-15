#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
 
// --- KONFIGURATION OLED ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
 
// --- KONFIGURATION SENSOR ---
const int trigPin = 26; 
const int echoPin = 27;
 
// --- KORRIGIERTES LED ARRAY (Hardware Mapping) ---
// Reihenfolge von UNTEN (Rot J) nach OBEN (Blau A)
// J(Rot)=0, I(Rot)=1, H(Gelb)=2, G(Gelb)=3, F(Gelb)=12, E(Grün)=13, D(Grün)=14, C(Grün)=16, B(Grün)=17, A(Blau)=25
const int allLeds[] = {0, 1, 2, 3, 12, 13, 14, 16, 17, 25};
const int numLeds = 10;
 
// Kalibrierung
const int TANK_LEER = 20; 
const int TANK_VOLL = 2;  
 
long duration;
int distance;
int tankLevelPercent;
 
void setup() {
  // WICHTIG: KEIN Serial.begin()! 
  // Das würde Pin 1 und 3 blockieren und die LEDs 2 und 4 einschalten.
  // 1. LEDs initialisieren
  for (int i = 0; i < numLeds; i++) {
    // Pin als Ausgang setzen
    pinMode(allLeds[i], OUTPUT);
    // Sofort ausschalten
    digitalWrite(allLeds[i], LOW); 
  }
 
  // 2. Sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
 
  // 3. OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    // Falls Display nicht geht, blinken wir als Warnung mit der obersten LED
    pinMode(25, OUTPUT);
    while(1) { digitalWrite(25, !digitalRead(25)); delay(100); }
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("System Start...");
  display.display();
  delay(1000); 
}
 
void loop() {
  // Messung
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  duration = pulseIn(echoPin, HIGH);
  //Schallgeschwindigkeit /10000 = 0.034. Hin und Rückweg => /2
  distance = duration * 0.034 / 2;
 
  tankLevelPercent = map(distance, TANK_LEER, TANK_VOLL, 0, 100);
  tankLevelPercent = constrain(tankLevelPercent, 0, 100);
 
  updateLedBar(tankLevelPercent);
  updateOled(distance, tankLevelPercent);
 
  delay(500); 
}
 
void updateLedBar(int percent) {
  int ledsToLight = map(percent, 0, 100, 0, numLeds);
 
  for (int i = 0; i < numLeds; i++) {
    if (i < ledsToLight) {
      digitalWrite(allLeds[i], HIGH);
    } else {
      digitalWrite(allLeds[i], LOW);
    }
  }
}
 
void updateOled(int dist, int percent) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("OPEN Tank");
  display.drawLine(0, 10, 128, 10, WHITE);
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.print(percent);
  display.print("%");
 
  display.setTextSize(1);
  display.setCursor(0, 45);
  display.print(dist);
  display.print(" cm");
 
  display.setCursor(0, 55); 
  if(percent < 10) display.print("! OPEN for Water !");
  else if (percent > 90) display.print("Nicht OPEN for Water");
 
  display.display();
}