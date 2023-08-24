#include <Ticker.h>
#include <SoftwareSerial.h>
#include "LoRa_E32.h"

#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Ticker.h>

#define BLYNK_TEMPLATE_ID "**********"
#define BLYNK_TEMPLATE_NAME "*************"
#include <BlynkSimpleEsp8266.h>

/*
 * LoRa E32-TTL-100
 * Send fixed broadcast transmission message to a specified channel.
 * https://www.mischianti.org/2019/11/10/lora-e32-device-for-arduino-esp32-or-esp8266-fixed-transmission-part-4/
 *
 * E32-TTL-100----- Arduino UNO or esp8266
 * M0         ----- 3.3v (To config) GND (To send) 7 (To dinamically manage)
 * M1         ----- 3.3v (To config) GND (To send) 6 (To dinamically manage)
 * TX         ----- RX PIN 2 (PullUP)
 * RX         ----- TX PIN 3 (PullUP & Voltage divider)
 * AUX        ----- Not connected (5 if you connect)
 * VCC        ----- 3.3v/5v
 * GND        ----- GND
 *
 */

LoRa_E32 e32ttl(2, 3, 4); // Config without connect AUX and M0 M1

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

void setupE32Service() {
  //TODO
  traceln("E32 service initialized.");
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
    setupE32Service();
}

void loop()
{
	if ( radio.dataAvailable() )
    {       
        traceln("Data available.");    

        //TODO Get payload from E32 into receive_payload
 		
        trace("Payload received: ");
        traceln(receive_payload.c_str());
		
        //TODO set payload data into charge and temperature variables.
        sendDataToBlynkServer();
        //sendDataToWifiServer();
        
        delay(100);
    }
}
