#include <Wire.h>
#include<ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"*/
#include "MAX30100_PulseOximeter.h"  //used arduino builtin MAX30100 lib (https://github.com/oxullo/Arduino-MAX30100)
#define REPORTING_PERIOD_MS 5000
#define WLAN_SSID          "Rana Villa"
#define WLAN_PASS        "Sp@9991109566"
#define AIO_SERVER        "io.adafruit.com"
#define AIO_SERVERPORT  1883                 
#define AIO_USERNAME "Lakshay1299"
#define AIO_KEY                "aio_EhLa80s0V8lXiD8DyWooTHEVFYUc"
// PulseOximeter is the higher-level interface to the sensor
PulseOximeter pox;
float BPM=0;
float SpO2 = 0;
uint32_t tsLastReport = 0;
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);        
Adafruit_MQTT_Publish bpm = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/bpm");
Adafruit_MQTT_Publish spo2 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/SpO2");
// Callback (registered below) fired when a pulse is detected
void onBeatDetected()
{
                Serial.println("Beat!");
}
void connect() {
  Serial.print("Connecting to MQTT... ");
  uint8_t ret;
  while ((ret = mqtt.connect()) != 0) {
    switch (ret) {
      case 1: Serial.println(F("Wrong protocol")); break;
      case 2: Serial.println(F("ID rejected")); break;
      case 3: Serial.println(F("Server unavail")); break;
      case 4: Serial.println(F("Bad user/pass")); break;
      case 5: Serial.println(F("Not authed")); break;
      case 6: Serial.println(F("Failed to subscribe")); break;
      default: Serial.println(F("Connection failed")); break;
    }
    if(ret >= 0)
      mqtt.disconnect();

    Serial.println(F("Retrying connection..."));
    delay(10000);
  }
  Serial.println("MQTT Connected!");
}

void setup()
{
                Serial.begin(115200);
                Serial.print(F("Connecting to "));
                Serial.println(WLAN_SSID);
                WiFi.begin(WLAN_SSID, WLAN_PASS);
                while (WiFi.status() != WL_CONNECTED) {
                  delay(500);
                  Serial.print(F("."));
                }
                 Serial.println();
                 Serial.println(F("WiFi connected"));
                 Serial.println(F("IP address: "));
                 Serial.println(WiFi.localIP());

                 // connect to adafruit io
                 connect();

                 Serial.print("Initializing Pulse Oximeter..");
                 pinMode(16, OUTPUT);
                  if (!pox.begin())
                  {
                       Serial.println("FAILED");
                       for(;;);
                  }
                  else
                  {
                       Serial.println("SUCCESS");
                  }
}
void loop() {
  pox.update();
  BPM = pox.getHeartRate();
  SpO2 = pox.getSpO2();
   
  if (millis() - tsLastReport > REPORTING_PERIOD_MS)
    {
        pox.setOnBeatDetectedCallback(onBeatDetected);
        Serial.print("Heart rate:");
        Serial.print(BPM);
        Serial.print(" bpm / SpO2:");
        Serial.print(SpO2);
        Serial.println(" %");

        if (! bpm.publish(BPM)) {                     //Publish to Adafruit
          Serial.println(F("Failed"));
        } 
        if (! spo2.publish(SpO2)) {                     //Publish to Adafruit
          Serial.println(F("Failed"));
        }
        else {
          Serial.println(F("Sent!"));
        }  
        tsLastReport = millis();
    }
}
