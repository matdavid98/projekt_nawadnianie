#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>


const char* ssid = "Mi 10T";
const char* password = "aaaaaaaa";


const char* serverName = "http://felixl120.eu.pythonanywhere.com/pomiary";
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;
 StaticJsonDocument<200> doc;
 String wyjscie;


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
  doc["nawodnienie"]=30.20;
  doc["temperatura"]=26.00;
  doc["naslonecznienie"]=50.12;
  doc["roslina"]=3;
  
  serializeJson(doc,  wyjscie);
}

void loop() {
  
  if ((millis() - lastTime) > timerDelay) {
  
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
           
      http.begin(serverName);     
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(wyjscie);

       
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
          
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}
