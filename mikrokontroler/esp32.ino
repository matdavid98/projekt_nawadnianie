#include <ArduinoJson.h>
#include <DHT.h>
#include <Time.h>
#include <TimeLib.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <esp_sleep.h>
#include "driver/adc.h"
#include <esp_bt.h>
#include <esp_wifi.h>


#define DIR_PIN 8
#define STEP_PIN 9
#define DHT11_PIN 7
#define uS_TO_S_FACTOR 1000000
#define TIME_TO_SLEEP  300 
const char* ssid = "Mi 10T";
const char* password = "aaaaaaaa";
String serverName = "http://felixl120.eu.pythonanywhere.com";
int roslina = 1;
DHT dht;
int kod=0;
time_t czas_podlania=0;
time_t czas_wybudzenia=0;
time_t przedzial = 600;


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
  dht.setup(DHT11_PIN);
  pinMode(DIR_PIN, OUTPUT); 
  pinMode(STEP_PIN, OUTPUT); 
  disableBluetooth();
  aktualny();
  czas_wybudzenia=now();
  }

void loop() {

 if(abs(czas_wybudzenia - now()) <= przedzial){
    senddata();
    if(kod>0 && abs(czas_podlania-now())<=przedzial){
     podlej(kod);
    }
    kod = gettime();
    aktualny();
    obliczczas();
    Serial.println(czas_wybudzenia);
    lightsleep();
   }

   
 }



int gettime(){
  WiFiClient client;
  HTTPClient http;
  tmElements_t czytelnyczas;
  StaticJsonDocument<200> doc;
  String path = serverName+ "/czasy/najblizszy/"+String(roslina);      
  
  String c="{}";
  http.begin(client, path);
  int httpResponseCode = http.GET();

  if(httpResponseCode==200){
    
  c=http.getString();
  Serial.println(c);
  http.end();
  deserializeJson(doc, c);
  int id=doc["id"];
  String czas=doc["data_operacji"];
  
  String temp = czas.substring(0,4);
  int rok=temp.toInt();

  temp = czas.substring(5,7);
  int  miesiac=temp.toInt();
  
  temp = czas.substring(8,10);
  int  dzien=temp.toInt();
  
  temp = czas.substring(11,13);
  int  godzina=temp.toInt();
  
  temp = czas.substring(14,16);
  int  minuta=temp.toInt();
  
  temp = czas.substring(17);
  int  sekunda=temp.toInt();

  czytelnyczas.Second=sekunda;
  czytelnyczas.Minute =minuta;
  czytelnyczas.Hour  =godzina;
  czytelnyczas.Day   =dzien;
  czytelnyczas.Month =miesiac;
  czytelnyczas.Year  =rok-1970;
  czas_podlania=makeTime(czytelnyczas);

  return id;
  }
  else {
  http.end();
  return -1;
  }
  
}

int aktualny(){
  WiFiClient client;
  HTTPClient http;
  String path = serverName+ "/todo/api/v1.0/time";             
  String c={};
  
  http.begin(client, path);
  int httpResponseCode = http.GET();

  if(httpResponseCode==200){
  c=http.getString();
  Serial.println(c);
  http.end();
  String temp = c.substring(1,5);
  int rok=temp.toInt();

  temp = c.substring(6,8);
  int  miesiac=temp.toInt();
  
  temp = c.substring(9,11);
  int  dzien=temp.toInt();
  
  temp = c.substring(12,14);
  int  godzina=temp.toInt();
  
  temp = c.substring(15,17);
  int  minuta=temp.toInt();
  
  temp = c.substring(18,20);
  int  sekunda=temp.toInt();

  
  setTime(godzina,minuta,sekunda,dzien,miesiac,rok);
  return 1;
  
  }
  else {
    http.end();
    return -1;
  }
}

int senddata(){
  WiFiClient client;
  HTTPClient http;
  StaticJsonDocument<200> doc;
  String wyjscie;
  String path = serverName+ "/pomiary";
  doc["nawodnienie"]=getwilgotnosc();
  doc["temperatura"]=gettemperatura();
  doc["naslonecznienie"]=getnaslonecznienie();
  doc["roslina"]=roslina;
  serializeJson(doc,  wyjscie);
  http.begin(client, path);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(wyjscie);              
  http.end();

  Serial.println(httpCode);
  if(httpCode>0) return httpCode;
  else return -1;
}

int changedata(int id){
  if (id>0){
  WiFiClient client;
  HTTPClient http;
  time_t t=now()+ 10700;
  String c;
  StaticJsonDocument<200> doc;
  String wyjscie;
  doc["roslina"]=1;
  //2021-04-08T17:17:29
  doc["data_operacji"]=String(year(t))+"-"+String(month(t))+"-"+String(day(t))+"T"+String(hour(t))+":"+String(minute(t))+":"+String(second(t));
  serializeJson(doc,  wyjscie);
  String path = serverName+"/czasy/"+String(id);
  http.begin(client, path);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.PUT(wyjscie);              
  http.end();                     
  
  Serial.println(httpCode);
  if(httpCode>0) return httpCode;
  else return -1;
  }
  else return -1;
}

float getnaslonecznienie(){
  int wartosc=0;
  float naslonecznienie=0.0;
  wartosc=analogRead(A0);
  naslonecznienie=(wartosc*100.0)/1024.0;
  return naslonecznienie;
}

float gettemperatura(){
  float temperatura=0;
  temperatura=(float)dht.getTemperature();
  return temperatura;
}  

float getwilgotnosc2(){
  float wilg=0;
  wilg=(float)dht.getHumidity();
  return wilg;
}

float getwilgotnosc(){
 int wartosc = analogRead(A3);
 float wilgotnosc = (wartosc*100.0)/1024.0;
 return wilgotnosc;
}


void podlej(int id){
  float wilgotnosc=getwilgotnosc();
  if(wilgotnosc>80.0){
    changedata(id);
  }
  else if(wilgotnosc>60.0){
    time_t timerr=now();
    while(timerr+10>now()){
    rotateDeg(360,0.1);
    }
    
  }
  else if(wilgotnosc>30.0){
    time_t timerr=now();
    while(timerr+20>now()){
    rotateDeg(360,0.1);
    }
    
  }
  else {
    time_t timerr=now();
    while(timerr+30>now()){
    rotateDeg(360,0.1);
    }
    
  }
  
}


void obliczczas(){
  if(czas_podlania-now()<10700 && czas_podlania>now() ){
    czas_wybudzenia=czas_podlania;
  }
  else{
    czas_wybudzenia=now()+10700;
  }

}

void rotate(int steps, float speed){ 
  //rotate a specific number of microsteps (8 microsteps per step) - (negitive for reverse movement)
  //speed is any number from .01 -> 1 with 1 being fastest - Slower is stronger
  int dir = (steps > 0)? HIGH:LOW;
  steps = abs(steps);

  digitalWrite(DIR_PIN,dir); 

  float usDelay = (1/speed) * 70;

  for(int i=0; i < steps; i++){ 
    digitalWrite(STEP_PIN, HIGH); 
    delayMicroseconds(usDelay); 

    digitalWrite(STEP_PIN, LOW); 
    delayMicroseconds(usDelay); 
  } 
} 

void rotateDeg(float deg, float speed){ 
  //rotate a specific number of degrees (negitive for reverse movement)
  //speed is any number from .01 -> 1 with 1 being fastest - Slower is stronger
  int dir = (deg > 0)? HIGH:LOW;
  digitalWrite(DIR_PIN,dir); 

  int steps = abs(deg)*(1/0.225);
  float usDelay = (1/speed) * 70;

  for(int i=0; i < steps; i++){ 
    digitalWrite(STEP_PIN, HIGH); 
    delayMicroseconds(usDelay); 

    digitalWrite(STEP_PIN, LOW); 
    delayMicroseconds(usDelay); 
  } 
}

void lightsleep(){
   time_t SleepSecs=czas_wybudzenia-now();
   Serial.println(SleepSecs);
   esp_sleep_enable_timer_wakeup(SleepSecs * uS_TO_S_FACTOR);
   disableWiFi();
   Serial.println("Going to light-sleep now");
  Serial.flush(); 
   esp_light_sleep_start();
   enableWiFi();
}

void disableWiFi(){
    adc_power_off();
    WiFi.disconnect(true);  // Disconnect from the network
    WiFi.mode(WIFI_OFF);    // Switch WiFi off
 
    Serial2.println("");
    Serial2.println("WiFi disconnected!");
}
void enableWiFi(){
    adc_power_on();
    WiFi.disconnect(false);  // Reconnect the network
    WiFi.mode(WIFI_STA);    // Switch WiFi off
 
    Serial2.println("START WIFI");
    WiFi.begin(ssid, password);
 
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial2.print(".");
    }
 
    Serial2.println("");
    Serial2.println("WiFi connected");
    Serial2.println("IP address: ");
    Serial2.println(WiFi.localIP());
}
void disableBluetooth(){
    btStop();
    esp_bt_controller_disable();
    delay(1000);
    Serial2.println("BT STOP");
}
