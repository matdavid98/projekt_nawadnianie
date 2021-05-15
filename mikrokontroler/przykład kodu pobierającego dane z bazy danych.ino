#include <WiFi.h>
#include <HTTPClient.h>
//#include <Arduino_JSON.h>
#include <ArduinoJson.h>

const char* ssid = "Mi 10T";
const char* password = "aaaaaaaa";
const char* serverName = "http://felixl120.eu.pythonanywhere.com/";


unsigned long lastTime = 0;
unsigned long timerDelay = 5000;
String sensorReadings;
float sensorReadingsArr[3];

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    
    if(WiFi.status()== WL_CONNECTED){
              
      sensorReadings = httpGETRequest(serverName);
      Serial.println(sensorReadings);
   StaticJsonDocument<192> doc;

DeserializationError error = deserializeJson(doc, sensorReadings);

if (error) {
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
  return;
}

const char* czas_dodania = doc["czas_dodania"]; // "2021-04-08T17:17:29"
const char* gatunek = doc["gatunek"]; // "nie wiem"
int id = doc["id"]; // 1
const char* nazwa = doc["nazwa"]; // "kaktus"

  Serial.println(gatunek);
  Serial.println(nazwa);
  Serial.println(czas_dodania);
  Serial.println(id);
  Serial.println();
   
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

String httpGETRequest(const char* serverName) {
  HTTPClient http;
    
  http.begin(serverName);
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();

  return payload;
}
