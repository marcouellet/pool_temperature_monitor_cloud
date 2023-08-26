#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Ticker.h>
#include "data_transfer_cc1101.h"
#include "trace.h"

#define BLYNK_TEMPLATE_ID "**********"
#define BLYNK_TEMPLATE_NAME "*************"
#include <BlynkSimpleEsp8266.h>

#define RADIO_CHANNEL         16
#define DEVICE_ADDRESS        21 // this device

const int maxbauds = 115200;
int charge = 0;
int temperature = 0;
int isLowVoltage = 0;
int timeToSleep = 0;
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

DataTransferCC1101 cc1101(CFREQ_922, RADIO_CHANNEL, DEVICE_ADDRESS, 10);

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
    cc1101.setupForReceive();
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
	if ( cc1101.isDataAvailable() )
    {  
        DataTransferMessage* message = cc1101.getData();
        if (message != NULL) {
            if (strcmp(message->type, MESSAGE_TYPE_DATA) == 0) {
                temperature = message->temperature;
                charge = message->charge;
                isLowVoltage = message->isLowVoltage;
                //sendDataToWifiServer();
                sendDataToBlynkServer();
                delay(100);
            }
        }     
    }
}
