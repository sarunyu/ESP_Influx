#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "DHT.h"
#include <OneWire.h>

#define DHTPIN 14
#define DHTTYPE DHT22
#define DS18B20PIN D2

OneWire  ds(2);
ADC_MODE(ADC_VCC);

unsigned long previousMillis = 0;
const long interval = 10000;
bool first = false;
bool failed = false;
int cout=0;
float sum_t=0;
float sum_h=0;
float t_av=0;
float h_av=0;

const char* ssid     = "Me";
const char* password = "12345600";

const char* database   = "smartfarm";
const char* type   = "nodesensor";
const char* region   = "maesariang";



DHT dht(DHTPIN, DHTTYPE);
HTTPClient http;

void setup() {
  
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(D2, OUTPUT); 
  digitalWrite(D2,LOW);
  pinMode(D4, INPUT_PULLUP); 
  dht.begin();
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  
  Serial.println();
  Serial.println();
  delay(1000);

}

void loop() {
  unsigned long currentMillis = millis();
  if( currentMillis - previousMillis >= interval || !first ||failed) {
    previousMillis = currentMillis;   

 digitalWrite(BUILTIN_LED, LOW);
  first = true;
  
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

      Serial.print("Humidity: ");
      Serial.print(h);
      Serial.print(" %\t");
      Serial.print("Temperature: ");
      Serial.print(t);
      Serial.print(" *C ");
 
  if (isnan(h) || isnan(t)) {
    failed = true;
    Serial.println("Error");
    delay(1000);
  }else{
     failed = false;
      sum_t = sum_t +t;
      sum_h = sum_h +h;
      cout = cout +1; 
      Serial.println(cout);
      Serial.println(ESP.getVcc());
   if(cout > 3){
      h_av = sum_h/4;
      t_av = sum_t/4;
      
      Serial.println(sum_t);
      Serial.println(sum_h);

      Serial.println(t_av);
      Serial.println(h_av);
      
      
      
      Serial.print("Humidity: ");
      Serial.print(h);
      Serial.print(" %\t");
      Serial.print("Temperature: ");
      Serial.print(t);
      Serial.print(" *C ");
      String  url = "http://45.77.32.210:8086/write?db=";
              url += database;

      String  message="weather2,type=";
              message+=type;
              message+=",region=";
              message+=region;
              message+=" temp=";
              message+=t_av;
              message+=",humid=";
              message+=h_av;
              message+=",heap=";
              message+=ESP.getFreeHeap(); 

      
      Serial.println(url); 
      Serial.println(message);   
      
      http.begin(url); //HTTP
      

      int httpCode = http.POST(message);
      if(httpCode) {
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
          if(httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          //Serial.println(payload);
          }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

        http.end();
      cout = 0;
      sum_t = 0;
      sum_h = 0;
      }

  
  

  digitalWrite(BUILTIN_LED, HIGH);
  
  
    }
  }
}
  


  
