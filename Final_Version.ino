#include "Secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include "DHT.h"
#define DHTPIN 14     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 11
#define Bodenfeuchtigkeitssensor 34
#define Wasserstandssensor 35
#include <Adafruit_SSD1306.h>
#include <Wire.h>
int Eingabe;
int Pumpe =  19;
int led_Gruen = 4;
int Wasser ;
float Daten ;
float Bodenfeuchtigkeitssensor_Daten ;
float Fuellstand;
float h ;
float t;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

DHT dht(DHTPIN, DHTTYPE);

#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"
 
 
WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);
 
void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 
  Serial.println("Connecting to Wi-Fi");
 
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
 
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);
 
  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);
 
  // Create a message handler
  client.setCallback(messageHandler);
 
  Serial.println("Connecting to AWS IOT");
 
  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(100);
  }
 
  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return;
  }
 
  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
 
  Serial.println("AWS IoT Connected!");
}
 
void publishMessage()
{
  StaticJsonDocument<200> doc;
  doc["Humidity"] = h;
  doc["Temperature"] = t;
  doc["Bodenfeuchtigkeit:"] = Bodenfeuchtigkeitssensor_Daten;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
 
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}
 
void messageHandler(char* topic, byte* payload, unsigned int length)
{
  Serial.print("incoming: ");
  Serial.println(topic);
 
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["message"];
  Serial.println(message);
}
// Definiere die Profilnamen als Konstanten
const char* PROFILE_1 = "1 ###";
const char* PROFILE_2 = "2 ###";
const char* PROFILE_3 = "3 ###";
const char* PROFILE_4 = "4 ###";


// Definiere die Eingabe-Prompt-Message
const char* PROMPT_MESSAGE = "Bitte wÃ¤hlen Sie ein Profil (1-4): ";


void setup()
{
  Serial.begin(115200);
  connectAWS();
  dht.begin();
  pinMode(Pumpe, OUTPUT);
  pinMode(Bodenfeuchtigkeitssensor, INPUT);
  pinMode(Wasserstandssensor, INPUT);
  pinMode(led_Gruen, OUTPUT);
  digitalWrite(Pumpe, HIGH);
  Serial.print(PROMPT_MESSAGE);
  Serial.print(F("Init display "));
  while(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
// Address 0x3C for OLED-Display
  {
    Serial.print(". ");
  }
  Serial.println(". DONE");
  // Clear the buffer
  display.clearDisplay();
  display.setTextSize(1); // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0,0); // Start at top-left corner
  display.println("Waterplantation");
  display.display();
  delay(10);
}



void loop(){
Bodenfeuchtigkeitssensor_Daten = analogRead(Bodenfeuchtigkeitssensor);
Fuellstand = analogRead(Wasserstandssensor);
  if (Fuellstand >3100)
  {
    digitalWrite(led_Gruen, HIGH);
    Wasser = 1;
  }
  else
  {
    digitalWrite(led_Gruen, LOW);
    Wasser =0;
  }
  if(Bodenfeuchtigkeitssensor_Daten>3100 and Wasser <= 0)
  {
    digitalWrite(Pumpe, HIGH);
    digitalWrite(Pumpe, LOW);
    delay(10000);
    digitalWrite(Pumpe, HIGH);
    delay(30000);
     }
  else
  {
    digitalWrite(Pumpe, HIGH);
  }
 
  h = dht.readHumidity();
  t = dht.readTemperature();
 
 
  if (isnan(h) || isnan(t) )  // Check if any reads failed and exit early (to try again).
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));
  Serial.print(F("Bodenfeuchtigkeit: "));
  Serial.print(Bodenfeuchtigkeitssensor_Daten) ;

  display.setTextSize(1); // Normal 1:1 pixel scale
  display.clearDisplay(); //Delete old content
  display.setTextSize(1); // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(10,10); // Start at top-left corner
  display.print("Bodenfeuchtigkeit: ");
  display.setTextSize(1); // Normal 2:1 pixel scale
//display.setCursor(85,32);
if (Bodenfeuchtigkeitssensor_Daten<2500) {
    // Bodenfeuchtigkeit ist OK
    display.println("OK");
  } else {
    // Bodenfeuchtigkeit ist nicht OK
    display.println("nicht OK");
  } 
  display.println();
  display.display(); //At last show new content


  publishMessage();
  client.loop();
  delay(1000);
}
