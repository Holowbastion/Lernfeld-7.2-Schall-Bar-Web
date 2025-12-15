#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WebServer.h>
 
// --- KONFIGURATION WLAN ---
const char* ssid = "OvM-Raspi";
const char* password = "abcD1234";

// Webserver auf Port 80
WebServer server(80);
 
// --- KONFIGURATION OLED ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
 
// --- KONFIGURATION SENSOR ---
const int trigPin = 26;
const int echoPin = 27;
 
// --- LED ARRAY (Hardware Mapping) ---
// Pins 1 und 3 sind TX/RX - daher KEIN Serial.begin nutzen!
const int allLeds[] = {0, 1, 2, 3, 12, 13, 14, 16, 17, 25};
const int numLeds = 10;
 
// Kalibrierung
const int TANK_LEER = 20;
const int TANK_VOLL = 2;  

#define LOGO_WIDTH  80
#define LOGO_HEIGHT 40
 
// Bitmap Daten für "OPEN" Logo (80x40)
// Weißer Text auf schwarzem Grund
static const unsigned char PROGMEM logo_bmp[] = {
  // Zeile 0-9 (Oberer Rand - Schwarz)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 
  // Beginn des Textes "OPEN" (ungefähr mittig)
  // O (links), P, E, N (rechts)
  0x00, 0x0F, 0xF0, 0x00, 0xFF, 0xC0, 0x0F, 0xFC, 0x00, 0xE0, // Oberkante Buchstaben
  0x00, 0x3F, 0xFC, 0x01, 0xFF, 0xE0, 0x1F, 0xFE, 0x01, 0xF0,
  0x00, 0x7F, 0xFE, 0x03, 0xFF, 0xF0, 0x3F, 0xFF, 0x03, 0xF0,
  0x00, 0xF0, 0x0F, 0x03, 0x81, 0xF0, 0x3E, 0x07, 0x07, 0xF0,
  0x01, 0xC0, 0x03, 0x07, 0x00, 0xF8, 0x7C, 0x03, 0x0F, 0xF0,
  0x01, 0x80, 0x01, 0x07, 0x00, 0x78, 0x78, 0x00, 0x1F, 0xF0,
  0x03, 0x80, 0x01, 0x07, 0x00, 0x7C, 0x78, 0x00, 0x3B, 0xF0,
  0x03, 0x00, 0x00, 0x07, 0x00, 0x3C, 0x78, 0x00, 0x73, 0xF0,
  0x03, 0x00, 0x00, 0x07, 0x00, 0x3C, 0x78, 0x00, 0xE3, 0xF0,
  0x03, 0x00, 0x00, 0x07, 0x00, 0x3C, 0x7F, 0xFC, 0xC3, 0xF0,
  0x03, 0x00, 0x00, 0x07, 0x00, 0x3C, 0x7F, 0xFC, 0x83, 0xF0,
  0x03, 0x00, 0x00, 0x07, 0x00, 0x3C, 0x7F, 0xF9, 0x03, 0xF0,
  0x03, 0x00, 0x00, 0x07, 0x00, 0x3C, 0x78, 0x00, 0x03, 0xF0,
  0x03, 0x80, 0x01, 0x07, 0x00, 0x1E, 0x78, 0x00, 0x03, 0xF0,
  0x01, 0x80, 0x01, 0x07, 0x00, 0x00, 0x78, 0x00, 0x03, 0xF0,
  0x01, 0xC0, 0x03, 0x07, 0x00, 0x00, 0x78, 0x00, 0x03, 0xF0,
  0x00, 0xF0, 0x0F, 0x07, 0x00, 0x00, 0x7C, 0x03, 0x03, 0xF0,
  0x00, 0x7F, 0xFE, 0x07, 0x00, 0x00, 0x7F, 0xFF, 0x03, 0xF0,
  0x00, 0x3F, 0xFC, 0x07, 0x00, 0x00, 0x3F, 0xFF, 0x03, 0xF0,
  0x00, 0x0F, 0xF0, 0x07, 0x00, 0x00, 0x1F, 0xFE, 0x03, 0xF0,
  // Zeile 27-39 (Unterer Rand - Schwarz)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
 
long duration;
int distance;
int tankLevelPercent;
 
// --- 1. HTML SEITEN GENERATOR (Das Dashboard) ---
String getHtmlPage() {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<meta http-equiv=\"refresh\" content=\"2\">\n"; // Refresh alle 2 Sek
  ptr +="<title>OPEN Tank Monitor</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  // CSS für den Balken
  ptr +=".bar-container{width: 80%; background-color: #ddd; margin: 0 auto; height: 30px; border-radius: 5px;}\n";
  ptr +=".bar{height: 100%; background-color: #4CAF50; text-align: right; line-height: 30px; color: white; border-radius: 5px; transition: width 0.5s;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>OPEN Tank Monitor</h1>\n";
  ptr +="<p>Wasserstand: <strong>" + String(tankLevelPercent) + "%</strong></p>\n";
  // Der dynamische Balken
  ptr +="<div class=\"bar-container\"><div class=\"bar\" style=\"width:" + String(tankLevelPercent) + "%\"></div></div>\n";
  ptr +="<p>Sensor Abstand: " + String(distance) + " cm</p>\n";
  if(tankLevelPercent < 10) {
    ptr +="<h2 style='color:red;'>! OPEN for Water !</h2>\n";
  } else {
    ptr +="<h2 style='color:green;'>Status: OK</h2>\n";
  }
  ptr +="<p><small>API Endpoint: <a href='/data'>/data</a></small></p>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
 
// --- 2. API ENDPOINT (JSON) ---
void handleData() {
  String json = "{";
  json += "\"percent\": " + String(tankLevelPercent) + ",";
  json += "\"distance_cm\": " + String(distance) + ",";
  if(tankLevelPercent < 10) json += "\"status\": \"leer\"";
  else json += "\"status\": \"ok\"";
  json += "}";
  server.send(200, "application/json", json);
}
 
// --- ROUTEN HANDLER ---
void handleRoot() {
  server.send(200, "text/html", getHtmlPage());
}
 
void setup() {
  // KEIN Serial.begin()!
 
  // 1. LEDs initialisieren & aus
  for (int i = 0; i < numLeds; i++) {
    pinMode(allLeds[i], OUTPUT);
    digitalWrite(allLeds[i], LOW);
  }
 
  // 2. Sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
 
  // 3. OLED Start
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    pinMode(25, OUTPUT);
    while(1) { digitalWrite(25, !digitalRead(25)); delay(100); }
  }
  // 4. WLAN Verbindung
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Verbinde WLAN...");
  display.print(ssid);
  display.display();
 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.print(".");
    display.display();
  }
 
  // 5. Server starten
  server.on("/", handleRoot);    // Das Dashboard
  server.on("/data", handleData); // Die API
  server.begin();
 
  // 6. IP kurz anzeigen
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("WLAN Verbunden!");
  display.println("IP Adresse:");
  display.println(WiFi.localIP());
  display.display();
  delay(4000); // Zeit zum Ablesen
}
 
void loop() {
  server.handleClient(); // Anfragen bearbeiten
 
  // Messung
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
 
  tankLevelPercent = map(distance, TANK_LEER, TANK_VOLL, 0, 100);
  tankLevelPercent = constrain(tankLevelPercent, 0, 100);
 
  updateLedBar(tankLevelPercent);
  updateOled(distance, tankLevelPercent);
 
  delay(100); 
}
 
void updateLedBar(int percent) {
  int ledsToLight = map(percent, 0, 100, 0, numLeds);
  for (int i = 0; i < numLeds; i++) {
    if (i < ledsToLight) digitalWrite(allLeds[i], HIGH);
    else digitalWrite(allLeds[i], LOW);
  }
}
 
// DEIN GEWÜNSCHTES OLED DESIGN
void updateOled(int dist, int percent) {
  display.clearDisplay();
  // Titel
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("OPEN Tank");
  display.drawLine(0, 10, 128, 10, WHITE);
  // Prozent Groß
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.print(percent);
  display.print("%");

//Logo
  display.drawBitmap(45, 12, logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
 
  // Sensorwert Klein
  display.setTextSize(1);
  display.setCursor(0, 45);
  display.print(dist);
  display.print(" cm");
 
  // Statusmeldung Unten
  display.setCursor(0, 55);
  if(percent < 10) display.print("! OPEN for Water !");
  else if (percent > 90) display.print("Nicht OPEN for Water");
 
  display.display();
}