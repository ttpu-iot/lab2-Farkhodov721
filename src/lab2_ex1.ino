#include "Arduino.h"
#include "WiFi.h"
#include "HTTPClient.h"
#include <ArduinoJson.h>

// pins for LEDs
int redPin = 26;
int greenPin = 27;
int bluePin = 14;
int yellowPin = 12;

// wifi name
String wifi = "Wokwi-GUEST";
String api = "http://api.thingspeak.com/channels/3109942/feeds.json?results=1";

/*************************
 * SETUP
 */
void setup()
{
  // setup led pins
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  
  // turn off all leds at start
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, LOW);
  digitalWrite(yellowPin, LOW);

  // start serial
  Serial.begin(115200);
  Serial.println("Abdulaziz, Lab 2");

  // connect wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi.c_str());
  
  // wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println();
  Serial.print("Connected! IP: ");
  Serial.println(WiFi.localIP().toString());
}


/*************************
 * LOOP
 */
void loop() 
{
  // check if wifi is still connected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Lost wifi! Trying to connect...");
    WiFi.begin(wifi.c_str());
    delay(2000);
    return;
  }

  // get data from thingspeak
  HTTPClient http;
  http.begin(api.c_str());
  int result = http.GET();

  // check if we got the data
  if (result != 200) {
    Serial.print("Error: ");
    Serial.println(result);
    http.end();
    delay(5000);
    return;
  }

  // get the response
  String data = http.getString();
  Serial.println(data);

  // try to parse json
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, data);

  if (error) {
    Serial.println("Could not parse JSON!");
  } else {
    // get led states from json
    String red = doc["feeds"][0]["field1"].as<String>();
    String green = doc["feeds"][0]["field2"].as<String>();
    String blue = doc["feeds"][0]["field3"].as<String>();
    String yellow = doc["feeds"][0]["field4"].as<String>();
    
    // update leds
    digitalWrite(redPin, red == "HIGH" ? HIGH : LOW);
    digitalWrite(greenPin, green == "HIGH" ? HIGH : LOW);
    digitalWrite(bluePin, blue == "HIGH" ? HIGH : LOW);
    digitalWrite(yellowPin, yellow == "HIGH" ? HIGH : LOW);
  }

  http.end();
  delay(5000);
}
