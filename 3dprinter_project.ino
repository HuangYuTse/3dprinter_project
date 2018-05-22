#include <Adafruit_Sensor.h>
#include <DHT.h>
#include "GM1602lib.h"
#include <string>
/*溫濕度模組函式庫*/

#include <ESP8266WiFi.h>
/*WiFiServer 函式來自這裡*/
const int Pir=4;//人體感應器腳位D2
const int Relay=0;//繼電器腳位D3
const int Dht=2;//dht22腳位D4
const int Co=A0;//CO腳位
const char* ssid = "";
const char* password = "";

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);
GM1602 GM1602(Co,3);
DHT dht(Dht,DHT22);
void setup() {
  Serial.begin(115200);
  delay(10);
  ////////pinsetting//////////
  pinMode(Relay, OUTPUT);
  pinMode(Pir, INPUT);
  dht.begin();
  ////////////////////////////
  // Connect to WiFi network
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
  
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  // Match the request
  int val;
  if (req.indexOf("/relay/on") != -1)
    val = 0;
  else if (req.indexOf("/relay/off") != -1)
    val = 1;
  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }

  // I/O working
  digitalWrite(2, val);
  float h = dht.readHumidity();//Read humidity
  float t = dht.readTemperature();// Read temperature as Celsius
  float f = dht.readTemperature(true);// Read temperature as Fahrenheit
  double co = GM1602.ppm(); // Read co ppm
  int pir = digitalRead(Pir);
  client.flush();

  // json response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n{ \n";
  s += "\"relay\":"+String(val)+",\n";
  s += "\"Humit\":"+String(h,3)+",\n";
  s += "\"temperatureC\":"+String(t,3)+",\n";
  s += "\"temperatureF\":"+String(f,3)+",\n";
  s += "\"Pir\":"+String(co,3)+",\n";
  s += "\"gas\":"+String(pir)+",\n";
  s += "}\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}

