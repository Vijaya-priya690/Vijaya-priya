#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include "DHT.h"
#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <FirebaseESP32.h>


#define WIFI_SSID "1"
#define WIFI_PASSWORD "12345678"
#define FIREBASE_HOST "https://myiot-d9e55-default-rtdb.firebaseio.com"

/** The database secret is obsoleted, please use other authentication methods, 
 * see examples in the Authentications folder. 
*/
#define FIREBASE_AUTH "QEVcQPEaCADKVIvwZoBahgNFKr0uq4pVb06LGrnJ"



// Uncomment one of the lines below for whatever DHT sensor type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

// DHT Sensor
uint8_t DHTPin = 4; 
               
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);                

float Temperature;
float Humidity;
 
#define REPORTING_PERIOD_MS 1000
 

// Connections : SCL PIN - D1 , SDA PIN - D2 , INT PIN - D0
PulseOximeter pox;
 volatile boolean heartBeatDetected = false;

uint32_t tsLastReport = 0;

 //Define FirebaseESP32 data object
FirebaseData firebaseData;
FirebaseJson json;
 String path = "/data";

 void onBeatDetected()
{
    heartBeatDetected = true;
    Serial.println("Beat!");
     
}

void setup()
{
    Serial.begin(115200);
    
    pinMode(19, OUTPUT);
    pinMode(DHTPin, INPUT);

  dht.begin();              

   // initialize with the I2C addr 0x3C
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  

    Serial.print("Initializing Pulse Oximeter..");
 
  
 WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");

    
         // Clear the buffer.
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("OLED OK");
  display.display();
    if (!pox.begin())
    {
         Serial.println("FAILED");
         for(;;);
    }
    else
    {
         Serial.println("SUCCESS");
         pox.setOnBeatDetectedCallback(onBeatDetected);
    }
    // The default current for the IR LED is 50mA and it could be changed by uncommenting the following line.
        pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
}
   float bpm;
        float SpO2;

void loop()
{
    pox.update();
  
 
     Temperature = dht.readTemperature(); // Gets the values of the temperature
  Humidity = dht.readHumidity(); // Gets the values of the humidity 
    if (millis() - tsLastReport > REPORTING_PERIOD_MS)
    {
      
 
        Serial.print("Heart rate:");
        Serial.print(bpm);
        Serial.print("BPM / SpO2:");
        Serial.print(SpO2);
        Serial.println(" %");
          Serial.print("Temperature:");
           Serial.print(Temperature);
        Serial.println(" C");
        Serial.print("Humidity:");
         Serial.print(Humidity);
        Serial.println(" %");

          display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
   display.print("Heart rate   :");
   display.print(bpm);
   
  display.display();
  display.setCursor(0,15);
   display.print("BPM / SpO2   :");
   display.print(SpO2);

  display.display();
  display.setCursor(0,30);
   display.print("Temperature  :");
   display.print(Temperature);
  display.display();
  display.setCursor(0,45);
   display.print("Humidity     :");
   display.print(Humidity);
  display.display();

 
 
        tsLastReport = millis();
    }
}