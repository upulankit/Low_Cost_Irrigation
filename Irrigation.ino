#include <ESP8266WiFi.h>                                                    
#include <FirebaseArduino.h>                                                 //Library for Firebase
#include <DHT.h>                                                             //Library for DHT Sensor
String apiKey = "JCJ99N*****2OPXU";                                          //api key of thingspeak
const char* server = "api.thingspeak.com";
#define FIREBASE_HOST "smart-irrigation-*****.firebaseio.com"                // the project name address from firebase id
#define FIREBASE_AUTH "T2RoL9BfBluQC7SqC66fc9*******ZYkFHVVXd2u"             // the secret key generated from firebase
#define WIFI_SSID "WiFi_name"                                                // Wifi Credentials
#define WIFI_PASSWORD "WiFi_Password"                                    
#define DHTPIN D3                                                            // Digital Pin for DHT Sensor
#define DHTTYPE DHT22                                                        // select dht type as DHT 11 or DHT22
DHT dht(DHTPIN, DHTTYPE);  
const int ledPin = D0; 
WiFiClient client;
void setup() {
  Serial.begin(9600);
  pinMode(A0,INPUT);                                                         //Analog Pin for Soil Moisture Sensor
  pinMode(ledPin, OUTPUT);                 
  delay(1000);                
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                     
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());                                            
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);                              
  dht.begin();                                                               
}
void loop() { 
 float h = dht.readHumidity();
 float t = dht.readTemperature();                                          // Reading temperature as Celsius (the default)
 int temp = analogRead(A0); 
 float  m = ( 100.00 - ( (temp / 1023.00) * 100.00 ) );                    //Conversion of Analog value to moisture value
 if (isnan(h) || isnan(t)) {                                                
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
Serial.print("Humidity: ");  
Serial.print(h);
Serial.print("%  Temperature: ");  Serial.print(t);  Serial.print("°C ");
Firebase.setFloat ("Temp",t);                                             //Sending Temp value to Firebase
Firebase.setFloat ("Humidity",h);                                         //Sending Humidity value to Firebase
Serial.print("  Moisture: ");  Serial.print(m); Serial.println("%");
Firebase.setFloat ("Moisture",m);                                         //Sending Moisture value to Firebase
if (client.connect(server,80))                                            //Sending data to Thingspeak 
                      { 
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(t);
                             postStr +="&field2=";
                             postStr += String(h);
                             postStr +="&field3=";
                             postStr += String(m);
                             postStr += "\r\n\r\n";
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
                             Serial.println(" Sent to Thingspeak and Firebase.");
                        }
                    client.stop();
          Serial.println("Waiting...");
          Serial.println(Firebase.getString("Switch"));                         //Reading string value from Firebase 
          if(Firebase.getString("Switch")=="\"1\"")
            {
                digitalWrite(D0,LOW);                                           //Relay gets powered ON on LOW signal
            }
          else{
                digitalWrite(D0,HIGH);                                          //Relay gets powered ON on LOW signal
          }          
}
