#include <Arduino.h>
#line 1 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\wifi_board\\cc1101\\cc1101.ino"
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Ticker.h>
#include "cc1101.h"

#define BLYNK_TEMPLATE_ID "**********"
#define BLYNK_TEMPLATE_NAME "*************"
#include <BlynkSimpleEsp8266.h>

#define RADIO_CHANNEL         16
#define DEVICE_ADDRESS        21 // this device

const int maxbauds = 115200;
int charge = 0;
int temperature = 0;
bool disableTraceDisplay = false;
String  receive_payload;

// Wifi client section

const char* ssid = "**********";
const char* password = "**********";
const char* host = "**********";
const uint16_t port = 17;

// Blynk client section 

#define BLYNK_AUTH_TOKEN "****************************"

char blynkAuth[] = BLYNK_AUTH_TOKEN;
const char* blynkSsid = "**********";
const char* blynkPassword = "**********";
const int blynkVpinTemperature = 10;
const int blynkVpinCharge = 11;

CC1101 radio;

#line 39 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\wifi_board\\cc1101\\cc1101.ino"
void trace(const char * str);
#line 45 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\wifi_board\\cc1101\\cc1101.ino"
void traceln(const char * str);
#line 51 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\wifi_board\\cc1101\\cc1101.ino"
void setupWifiClient();
#line 69 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\wifi_board\\cc1101\\cc1101.ino"
void setupBlynkClient();
#line 76 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\wifi_board\\cc1101\\cc1101.ino"
void setupC1101Service();
#line 89 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\wifi_board\\cc1101\\cc1101.ino"
void sendDataToBlynkServer();
#line 94 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\wifi_board\\cc1101\\cc1101.ino"
void sendDataToWifiServer();
#line 131 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\wifi_board\\cc1101\\cc1101.ino"
void setup();
#line 147 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\wifi_board\\cc1101\\cc1101.ino"
void loop();
#line 39 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\wifi_board\\cc1101\\cc1101.ino"
void trace(const char * str) {
    if (!disableTraceDisplay) {
    Serial.print(str);
    }
}

void traceln(const char * str) {
    if (!disableTraceDisplay) {
    Serial.println(str);
    } 
}

void setupWifiClient() {
    /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        trace(".");
    }

    traceln("");
    traceln("WiFi connected");
    traceln("IP address: ");
    traceln(WiFi.localIP().toString().c_str());
}

void setupBlynkClient() {
    Blynk.begin(blynkAuth, blynkSsid, blynkPassword);

    traceln("");
    traceln("Blynk connected");
}

void setupC1101Service() {
  // Start RADIO
  while (!radio.begin(CFREQ_922, RADIO_CHANNEL, DEVICE_ADDRESS));   // channel 16! Whitening enabled 
   
  delay(1000); // Try again in 5 seconds
  //radio.printCConfigCheck();     

  radio.setRxAlways();
  radio.setRxState();    

  traceln("CC1101 radio initialized.");
}

void sendDataToBlynkServer() {
    Blynk.virtualWrite(blynkVpinTemperature, temperature); //virtual pin V10
    Blynk.virtualWrite(blynkVpinCharge, charge); //virtual pin V11
}

void sendDataToWifiServer() {
    char str[150];
    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    HTTPClient http;

    trace("[HTTP] begin...\n");
    // configure traged server and url
    sprintf(str, "http://%s/data", host);
    http.begin(client, str);  // HTTP
    http.addHeader("Content-Type", "application/json");

    trace("[HTTP] POST...\n");
    // start connection and send HTTP header and body
    sprintf(str, "{""temperature"": ""%s"", ""charge"": ""%s""}", temperature, charge);
    int httpCode = http.POST(str);

    // httpCode will be negative on error
    if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        sprintf(str, "[HTTP] POST... code: %d\n", httpCode);
        trace(str);
        // file found at server
        if (httpCode == HTTP_CODE_OK) {
            const String& payload = http.getString();
            traceln("received payload:\n<<");
            traceln(payload.c_str());
            traceln(">>");
        }
        } else {
            sprintf(str, "[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
            trace(str);
        }

    http.end();
}

void setup()
{
    char str[150];
    Serial.begin(maxbauds);   
    delay(500);

    traceln("Setup started");

    sprintf(str, "Baud rate = %d seconds", maxbauds);
    traceln(str);

    //setupWifiClient();
    setupBlynkClient();
    setupC1101Service();
}

void loop()
{
	if ( radio.dataAvailable() )
    {       
        traceln("Data available.");    

        receive_payload  = String(radio.getChars()); // pointer to memory location of start of string
        //trace("Payload size received: "); traceln(radio.getSize());
		
        trace("Payload received: ");
        traceln(receive_payload.c_str());
		
        //TODO set payload data into charge and temperature variables.
        sendDataToBlynkServer();
        //sendDataToWifiServer();
        
        delay(100);
    }
}

